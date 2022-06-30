#include "../drivers/gxconsole/dev_cons.h"
#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>
#include <sched.h>
#include <error.h>

extern char *KERNEL_SP;
extern struct Env *curenv;

/* Overview:
 * 	This function is used to print a character on screen.
 *
 * Pre-Condition:
 * 	`c` is the character you want to print.
 */
void sys_putchar(int sysno, int c, int a2, int a3, int a4, int a5)
{
	printcharc((char) c);
	return ;
}

/* Overview:
 * 	This function enables you to copy content of `srcaddr` to `destaddr`.
 *
 * Pre-Condition:
 * 	`destaddr` and `srcaddr` can't be NULL. Also, the `srcaddr` area
 * 	shouldn't overlap the `destaddr`, otherwise the behavior of this
 * 	function is undefined.
 *
 * Post-Condition:
 * 	the content of `destaddr` area(from `destaddr` to `destaddr`+`len`) will
 * be same as that of `srcaddr` area.
 */
void *memcpy(void *destaddr, void const *srcaddr, u_int len)
{
	char *dest = destaddr;
	char const *src = srcaddr;

	while (len-- > 0) {
		*dest++ = *src++;
	}

	return destaddr;
}

/* Overview:
 *	This function provides the environment id of current process.
 *
 * Post-Condition:
 * 	return the current environment id
 */
u_int sys_getenvid(void)
{
	return curenv->env_id;
}

/* Overview:
 *	This function enables the current process to give up CPU.
 *
 * Post-Condition:
 * 	Deschedule current process. This function will never return.
 *
 * Note:
 *  For convenience, you can just give up the current time slice.
 */
/*** exercise 4.6 ***/
void sys_yield(void)
{
	bcopy(KERNEL_SP - sizeof(struct Trapframe), 
			(void *)TIMESTACK - sizeof(struct Trapframe), 
			sizeof(struct Trapframe));
	sched_yield();
}

/* Overview:
 * 	This function is used to destroy the current environment.
 *
 * Pre-Condition:
 * 	The parameter `envid` must be the environment id of a
 * process, which is either a child of the caller of this function
 * or the caller itself.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 when error occurs.
 */
int sys_env_destroy(int sysno, u_int envid)
{
	/*
		printf("[%08x] exiting gracefully\n", curenv->env_id);
		env_destroy(curenv);
	*/
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0) {
		return r;
	}

	//printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

/* Overview:
 * 	Set envid's pagefault handler entry point and exception stack.
 *
 * Pre-Condition:
 * 	xstacktop points one byte past exception stack.
 *
 * Post-Condition:
 * 	The envid's pagefault handler will be set to `func` and its
 * 	exception stack will be set to `xstacktop`.
 * 	Returns 0 on success, < 0 on error.
 */
/*** exercise 4.12 ***/
int sys_set_pgfault_handler(int sysno, u_int envid, u_int func, u_int xstacktop)
{
	// Your code here.
	struct Env *env;
	int ret;
	
	if ((ret = envid2env(envid, &env,0)) < 0) {
		return ret;
	}

	env->env_pgfault_handler = func;
	env->env_xstacktop = xstacktop;

	return 0;
	//	panic("sys_set_pgfault_handler not implemented");
}

/* Overview:
 * 	Allocate a page of memory and map it at 'va' with permission
 * 'perm' in the address space of 'envid'.
 *
 * 	If a page is already mapped at 'va', that page is unmapped as a
 * side-effect.
 *
 * Pre-Condition:
 * perm -- PTE_V is required,
 *         PTE_COW is not allowed(return -E_INVAL),
 *         other bits are optional.
 *
 * Post-Condition:
 * Return 0 on success, < 0 on error
 *	- va must be < UTOP
 *	- env may modify its own address space or the address space of its children
 */
/*** exercise 4.3 ***/
int sys_mem_alloc(int sysno, u_int envid, u_int va, u_int perm)
{
	// Your code here.
	struct Env *env;
	struct Page *ppage;
	int ret;
	ret = 0;

	if (!(perm & PTE_V) || (perm & PTE_COW) || va >= UTOP) {
		return -E_INVAL;
	}
	if ((ret = envid2env(envid, &env, 1)) < 0) {
		return ret;
	}
	if ((ret = page_alloc(&ppage)) < 0) {
        return ret;
    }
    if ((ret = page_insert(env->env_pgdir, ppage, va, perm)) < 0) {
        return ret;
    }
	return ret;
}

/* Overview:
 * 	Map the page of memory at 'srcva' in srcid's address space
 * at 'dstva' in dstid's address space with permission 'perm'.
 * Perm must have PTE_V to be valid.
 * (Probably we should add a restriction that you can't go from
 * non-writable to writable?)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Note:
 * 	Cannot access pages above UTOP.
 */
/*** exercise 4.4 ***/
int sys_mem_map(int sysno, u_int srcid, u_int srcva, u_int dstid, u_int dstva,
				u_int perm)
{
	int ret;
	u_int round_srcva, round_dstva;
	struct Env *srcenv;
	struct Env *dstenv;
	struct Page *ppage;
	Pte *ppte;

	ppage = NULL;
	ret = 0;
	round_srcva = ROUNDDOWN(srcva, BY2PG);
	round_dstva = ROUNDDOWN(dstva, BY2PG);

    //your code here
	if (!(perm & PTE_V) || srcva >= UTOP || dstva >= UTOP) {
		return -E_INVAL;
	}
	if ((ret = envid2env(srcid, &srcenv, 0)) < 0) {
		return ret;
	}
	if ((ret = envid2env(dstid, &dstenv, 0)) < 0) {
		return ret;
	}
	if (!(ppage = page_lookup(srcenv->env_pgdir, round_srcva, &ppte))) {
		return -E_INVAL;
	}
    if ((ret = page_insert(dstenv->env_pgdir, ppage, round_dstva, perm)) < 0) {
        return ret;    
	}
	return ret;
}

/* Overview:
 * 	Unmap the page of memory at 'va' in the address space of 'envid'
 * (if no page is mapped, the function silently succeeds)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Cannot unmap pages above UTOP.
 */
/*** exercise 4.5 ***/
int sys_mem_unmap(int sysno, u_int envid, u_int va)
{
	// Your code here.
	int ret;
	struct Env *env;
	//	panic("sys_mem_unmap not implemented");
	if (va >= UTOP) {
		return -E_INVAL;
	}
	if ((ret = envid2env(envid, &env, 0)) < 0) {
		return ret;
	}
	page_remove(env->env_pgdir, va);
	return ret;
}

/* Overview:
 * 	Allocate a new environment.
 *
 * Pre-Condition:
 * The new child is left as env_alloc created it, except that
 * status is set to ENV_NOT_RUNNABLE and the register set is copied
 * from the current environment.
 *
 * Post-Condition:
 * 	In the child, the register set is tweaked so sys_env_alloc returns 0.
 * 	Returns envid of new environment, or < 0 on error.
 */
/*** exercise 4.8 ***/
int sys_env_alloc(void)
{
	// Your code here.
	int r;
	struct Env *e;

	if ((r = env_alloc(&e, curenv->env_id)) < 0) {
		return r;
	}	
	e->env_tf = *((struct Trapframe *)KERNEL_SP - 1);
	e->env_tf.pc = e->env_tf.cp0_epc;
	e->env_tf.regs[2] = 0;
	e->env_status = ENV_NOT_RUNNABLE;
	e->env_pri = curenv->env_pri;

	return e->env_id;
	//	panic("sys_env_alloc not implemented");
}

/* Overview:
 * 	Set envid's env_status to status.
 *
 * Pre-Condition:
 * 	status should be one of `ENV_RUNNABLE`, `ENV_NOT_RUNNABLE` and
 * `ENV_FREE`. Otherwise return -E_INVAL.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if status is not a valid status for an environment.
 * 	The status of environment will be set to `status` on success.
 */
/*** exercise 4.14 ***/
int sys_set_env_status(int sysno, u_int envid, u_int status)
{
	// Your code here.
	struct Env *env;
	int ret;

	if (status != ENV_RUNNABLE && status != ENV_NOT_RUNNABLE && status != ENV_FREE) {
		return -E_INVAL;
	}
	
	if ((ret = envid2env(envid, &env, 0)) < 0) {
		return ret;
	}

	if (env->env_status != ENV_RUNNABLE && status == ENV_RUNNABLE) {
		LIST_INSERT_HEAD(&env_sched_list[0], env, env_sched_link);
	}
	else if (env->env_status == ENV_RUNNABLE && status != ENV_RUNNABLE) {
		LIST_REMOVE(env, env_sched_link);
	}
	env->env_status = status;
	return 0;
	//	panic("sys_env_set_status not implemented");
}

/* Overview:
 * 	Set envid's trap frame to tf.
 *
 * Pre-Condition:
 * 	`tf` should be valid.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if the environment cannot be manipulated.
 *
 * Note: This hasn't be used now?
 */
int sys_set_trapframe(int sysno, u_int envid, struct Trapframe *tf)
{

	return 0;
}

/* Overview:
 * 	Kernel panic with message `msg`.
 *
 * Pre-Condition:
 * 	msg can't be NULL
 *
 * Post-Condition:
 * 	This function will make the whole system stop.
 */
void sys_panic(int sysno, char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	panic("%s", TRUP(msg));
}

/* Overview:
 * 	This function enables caller to receive message from
 * other process. To be more specific, it will flag
 * the current process so that other process could send
 * message to it.
 *
 * Pre-Condition:
 * 	`dstva` is valid (Note: NULL is also a valid value for `dstva`).
 *
 * Post-Condition:
 * 	This syscall will set the current process's status to
 * ENV_NOT_RUNNABLE, giving up cpu.
 */
/*** exercise 4.7 ***/
void sys_ipc_recv(int sysno, u_int dstva)
{
	if (dstva >= UTOP) {
		return;
	}
	curenv->env_ipc_recving = 1;
	curenv->env_ipc_dstva = dstva;
	if (sys_set_env_status(0, 0, ENV_NOT_RUNNABLE) < 0) {
		panic("sys_ipc_recv: fail to set status.\n");
	}
	sys_yield();	//Giving up CPU, sched_yield again.
}

/* Overview:
 * 	Try to send 'value' to the target env 'envid'.
 *
 * 	The send fails with a return value of -E_IPC_NOT_RECV if the
 * target has not requested IPC with sys_ipc_recv.
 * 	Otherwise, the send succeeds, and the target's ipc fields are
 * updated as follows:
 *    env_ipc_recving is set to 0 to block future sends
 *    env_ipc_from is set to the sending envid
 *    env_ipc_value is set to the 'value' parameter
 * 	The target environment is marked runnable again.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Hint: the only function you need to call is envid2env.
 */
/*** exercise 4.7 ***/
int sys_ipc_can_send(int sysno, u_int envid, u_int value, u_int srcva,
					 u_int perm)
{

	int r;
	struct Env *e;
	struct Page *p;
	
	if (srcva >= UTOP) {
		return -E_INVAL;
	}
	if ((r = envid2env(envid, &e, 0)) < 0) {
		return r;
	}
	if (!(e->env_ipc_recving)) {
		return -E_IPC_NOT_RECV;
	}
	
	// Many to do
	if (srcva) {
		if (!(p = page_lookup(curenv->env_pgdir, srcva, NULL))) {
			return -E_INVAL;
		}	
		if ((r = page_insert(e->env_pgdir, p, e->env_ipc_dstva, perm)) < 0) {
			return r;
		}
	}
	
	e->env_ipc_recving = 0;
	e->env_ipc_value = value;
	e->env_ipc_from = curenv->env_id;
	e->env_ipc_perm = perm;
	
	if ((r = sys_set_env_status(0, envid, ENV_RUNNABLE)) < 0) {
		return r;
	}
	return 0;
}
/* Overview:
 * 	This function is used to write data to device, which is
 * 	represented by its mapped physical address.
 *	Remember to check the validity of device address (see Hint below);
 * 
 * Pre-Condition:
 *      'va' is the starting address of source data, 'len' is the
 *      length of data (in bytes), 'dev' is the physical address of
 *      the device
 * 	
 * Post-Condition:
 *      copy data from 'va' to 'dev' with length 'len'
 *      Return 0 on success.
 *	Return -E_INVAL on address error.
 *      
 * Hint: Use ummapped segment in kernel address space to perform MMIO.
 *	 Physical device address:
 *	* ---------------------------------*
 *	|   device   | start addr | length |
 *	* -----------+------------+--------*
 *	|  console   | 0x10000000 | 0x20   |
 *	|    IDE     | 0x13000000 | 0x4200 |
 *	|    rtc     | 0x15000000 | 0x200  |
 *	* ---------------------------------*
 */
 /*** exercise 5.1 ***/

static int checkRange(u_int va, u_int len) {
	int i;
	for (i = 0; i < 3; i++) {
		u_int startAddr;
		u_int length;
		switch (i) {
			case 0:
				startAddr = 0x10000000;
				length = 0x20;
				break;
			case 1:
				startAddr = 0x13000000;
				length = 0x4200;
				break;
			case 2:
				startAddr = 0x15000000;
				length = 0x200;
				break;
			default:
				break;
		}
		if ((startAddr <= va) && ((va + len) <= (startAddr + length))) {
			return 0;
		}
	}
	return -E_INVAL;
}

int sys_write_dev(int sysno, u_int va, u_int dev, u_int len)
{
	int r;
	if ((r = checkRange(dev, len)) < 0) {
		return r;	
	}
	bcopy((void *)va, (void *)dev + 0xa0000000, len);
	return 0;
}

/* Overview:
 * 	This function is used to read data from device, which is
 * 	represented by its mapped physical address.
 *	Remember to check the validity of device address (same as sys_write_dev)
 * 
 * Pre-Condition:
 *      'va' is the starting address of data buffer, 'len' is the
 *      length of data (in bytes), 'dev' is the physical address of
 *      the device
 * 
 * Post-Condition:
 *      copy data from 'dev' to 'va' with length 'len'
 *      Return 0 on success, < 0 on error
 *      
 * Hint: Use ummapped segment in kernel address space to perform MMIO.
 */
 /*** exercise 5.1 ***/
int sys_read_dev(int sysno, u_int va, u_int dev, u_int len)
{
        int r;
		if ((r = checkRange(dev, len)) < 0) {
			return r;	
		}
		bcopy((void *)dev + 0xa0000000, (void *)va, len);
		return 0;
}

/*
int sys_envar(int sysno, char *name, char *value, u_int op) {
	const int MOD = 256;
	static char name_table[256][64];
	static char value_table[256][256];

	if (op == 4) {
		char **name_list = name;
		char **value_list = value;
		int pos = 0, i;
		for (i = 0; i < 256; i++) {
			if (name_table[i][0]) {
				name_list[pos] = name_table[i];
				value_list[pos] = value_table[i];
				pos++;	
			}	
		}	
		name_list[pos] = 0;
	}

	u_int pos = strhash(name);

	while (name_table[pos][0]) {
		if (strcmp(name_table[pos], name) == 0) {
			if (op == 0) {
				return 0;	
			}	
			break;
		} else {
			pos++;
			if (pos == MOD) {
				pos = 0;	
			}	
		}
	}	

	switch (op) {
		case 0:
			strcpy(name_table[pos], name);
			strcpy(value_table[pos], value);
			break;
		case 1:
			if (strcmp(name_table[pos], name)) {
				return -E_ENVAR_NOT_FOUND;	
			}
			strcpy(value, value_table[pos]);
			break;
		case 2:
			if (strcmp(name_table[pos], name)) {
				return -E_ENVAR_NOT_FOUND;	
			}
			strcpy(value_table[pos], value);
		case 3:
			if (strcmp(name_table[pos], name)) {
				return -E_ENVAR_NOT_FOUND;	
			}
			int p = 0;
			while (p < 64 && name_table[pos][p]) {
				name_table[pos][p++] = 0;	
			}
			p = 0;
			while (p < 256 && value_table[pos][p]) {
				value_table[pos][p++] = 0;	
			}
			break;
		default:
			break;	
	}
	return 0;
}
*/
int sys_envar(int sysno, char *name, char *value, u_int op, u_int isEnvar) {
    const int MOD = 1 << 8;
    static char name_table[1 << 8][64];
    static char value_table[1 << 8][256];
    static int is_readonly[1 << 8];
	static int definedAge[1 << 8];	// When the variable is declared, the age is ...
	static int currentAge;

	// If definedAge == -1, then it is an environment variable; else if defineAge == currentAge, then it is a local variable.

	
	if (strcmp(name, "curpath") == 0) {
		currentAge++;
	}
	/*

	if (currentAge == 1) {
		printf("Now you'are at parentShell!\n");	
	} else {
		printf("Now you'are at childShell!\n");
	}
	*/

    u_int pos = strhash(name);

    while (name_table[pos][0]) {
        if (strcmp(name_table[pos], name) == 0) {
			// FOUND
            if (op == 0) {
				return 0;
			}
            break;
        } else {
            ++pos;
            if (pos == MOD) pos = 0;
        }
    }

    if (op == 0) {
		// Create a new variable
		strcpy(name_table[pos], name);
		strcpy(value_table[pos], value);
		if (isEnvar == 1) {
			definedAge[pos] = -1;	
		} else {
			definedAge[pos] = currentAge;
		}
    } else if (op == 1) {
		// Get the value of NAME
		if (definedAge[pos] != currentAge && definedAge[pos] != -1) {
			// Not local && Not Environment
			printf("\033[0m\033[1;31mCurrent shell doesn't contain this variable!\n\033[0m");
			return -E_ENVAR_NOT_FOUND;	
		}
        if (strcmp(name_table[pos], name) != 0) { 
			printf("\033[0m\033[1;31mCurrent shell doesn't contain this variable!\n\033[0m");
			return -E_ENVAR_NOT_FOUND;
		}
        strcpy(value, value_table[pos]);
    } else if (op == 2) {
		// Set NAME-VALUE
        if (strcmp(name_table[pos], name) != 0) 
			return -E_ENVAR_NOT_FOUND;
        if (is_readonly[pos]) 
			return -E_ENVAR_READONLY;
		if (definedAge[pos] != -1 && definedAge[pos] != currentAge) {
			//Not environment or local variable, a.k.a other Shell's local variable
			return -E_ENVAR_NOT_FOUND;	
		}
		// Set variable
        strcpy(value_table[pos], value);
		if (isEnvar) {
			definedAge[pos] = -1;
		}
		else if (!isEnvar) {
			definedAge[pos] = currentAge;
		}
    } else if (op == 3) {
		// unset a non-read-only variable
        if (strcmp(name_table[pos], name)) 
			return -E_ENVAR_NOT_FOUND;
		if (is_readonly[pos]) {
			return -E_ENVAR_READONLY;	
		} else {
			definedAge[pos] = 0;
        	int p = 0;
        	while (p < 64 && name_table[pos][p]) {
				name_table[pos][p] =0;
				p++;
			}
        	p = 0;
        	while (p < 256 && value_table[pos][p]) {
				value_table[pos][p] = 0;
				p++;
			}
		}
    } else if (op == 4) {
		// List all the variable
        int pos = 0, i, j, cnt = 0;
        for (i = 0; i < MOD; ++i)
            if (name_table[i][0]) {
				if (strcmp(name_table[i], "curpath") == 0) {
					continue;	
				}
                printf("NAME: %s", name_table[i]);
				for (j = 0; j < (16 - strlen(name_table[i])); j++)
					printf(" ");
				printf("VALUE: %s", value_table[i]);
				for (j = 0; j < (16 - strlen(value_table[i])); j++)
					printf(" ");
				if (definedAge[i] == -1) {
					printf("\033[0m\033[1;34m--[ENVIRONMENT VARIABLE]\033[0m");
				} else {
					printf("\033[0m\033[1;32m--[LOCAL VARIABLE]\033[0m");	
				}
                if (is_readonly[i]) {
                    printf("\033[0m\033[1;35m--[READ_ONLY]\033[0m");
                }
                printf("\n");
                cnt++;
            }
        if (cnt == 0) {
            printf("\033[0m\033[1;33mHaven't declare any variable in this shell now.\n\033[0m");   
        }
    } else if (op == 5) {
		// Set an variable as read-only
        is_readonly[pos] = 1;
    }

    return 0;
}
