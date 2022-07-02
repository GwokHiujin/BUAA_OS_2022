---
# try also 'default' to start simple
theme: seriph
background: https://source.unsplash.com/collection/94734566/1920x1080
# apply any windi css classes to the current slide
class: 'text-center'
highlighter: shiki
# show line numbers in code blocks
lineNumbers: true
download: truet

exportFilename: Report_lab6_challenge.pdf
remoteAssets: false
# some information about the slides, markdown enabled
info: |
  ## Slidev Starter Template
  Presentation slides for developers.

  Learn more at [Sli.dev](https://sli.dev)
# persist drawings in exports and build
drawings:
  persist: false
# use UnoCSS (experimental)
# css: unocss
---

# **Report: Lab6-challenge**

郭晓燕  20373543

<div class="pt-12">
  <span @click="$slidev.nav.next" class="px-2 py-1 rounded cursor-pointer" hover="bg-white bg-opacity-10">
    Press Space for next page <carbon:arrow-right class="inline"/>
  </span>
</div>

<!--
The last comment block of each slide will be treated as slide notes. It will be visible and editable in Presenter Mode along with the slide. [Read more in the docs](https://sli.dev/guide/syntax.html#notes)
-->

---
layout: image-right
image: https://source.unsplash.com/collection/94734566/1920x1080
---

# **目录**

Content of this report.

<br>

- **任务列表**

- **前置工作**
- **特殊字符特判**
- **外部命令扩展**
- **内部命令扩展**
- **测试工作** & **问题及解决方案**

---

# **任务列表**

In lab6-challenge, I have finished the following tasks.

<br>

### **1. 特殊字符特判**    [basic]

<br>

- **&**：通过  `[command] & `实现后台运行
- **;**：通过 `[command1]; [command2]` 实现一行多命令
- **“”**：通过 `“[String]”` 实现引号支持，使得双引号内的内容被看作一个简单字符串

<br>

<br>

### **2. 外部命令扩展**    [basic]

<br>

- **tree**：通过 `tree` 递归地列出当前目录下文件的的树形结构
- **mkdir**：通过 `mkdir [[PATH] DIRNAME]` 创建新的目录文件
- **touch**：通过 `touch [[PATH] FILENAME]` 创建新的常规文件

---

# **任务列表**

In lab6-challenge, I have finished the following tasks.

<br>

### **3. 内部命令扩展**    [Normal & Extra]

<br>

**history**：历史命令功能

- 实现将 shell 中输入过的指令保存到 `.history` 文件
- 通过 `history` 命令输出所有的历史指令
- 通过上下键回溯历史指令

<br>

**unset NAME**：解除变量定义

- 解除对 NAME 变量的定义：如果 NAME 不是只读变量，删除 NAME，否则给出相应的报错

---

# **任务列表**

In lab6-challenge, I have finished the following tasks.

<br>

### **3. 内部命令扩展**    [Normal & Extra]

<br>

**declare [-xr] [NAME [=VALUE]]**

- **-x**：表示将变量 NAME 导出为环境变量，否则为局部变量
    - 环境变量对子 shell 可见，也就是说在 shell 中输入 `sh` 启动一个子 shell 后，可以读
        取 NAME 的值
    - 局部变量对子 shell 不可见，也就是说在 shell 中输入 `sh` 启动一个子 shell 后，没有
        该局部变量，会给出相应报错
- **-r**：表示将变量 NAME 设为只读。只读变量不能被 `declare` 重新定义或被 `unset` 删除
- 如果没有 [-xr] 及 [NAME [=VALUE]] 部分，则输出当前 shell 的所有变量，包括局部变量
    和环境变量
- 支持并在执行诸如 `echo $variable` 指令时能显示正确的值

---
layout: image-right
image: https://source.unsplash.com/collection/94734566/1920x1080
---

# **前置工作**

Pre work of the lab.

- **完善文件系统功能**
    - 实现创建常规文件和目录的功能
    - 实现 O_APPEND

- **清除多余输出**
	- 关于进程和系统服务的调试信息
	- `fwritef` 和 `writef` 的多余空行

- **扩充 `string.c` 库以支持更多字符串操作**
    - `strlen`, `strcat`, `strhash` 等
    - 用户态和内核态都需要扩充

---


# **前置工作**

<br>

### **完善文件系统功能**

<br>

- 实现创建常规文件和目录的功能    - for `mkdir` && `touch` && `history`

    

    - 添加新的文件系统服务 `file_create`，为用户态支持 `create` 操作
- 创建文件需要：路径 (path + name) + 类型 (FTYPE_REG or FTYPE_DIR)
  
- 实现 O_APPEND 将内容写到文件末尾    -for `history`

---



# **前置工作**

<br>

### **完善文件系统功能**

<br>

- 实现创建常规文件和目录的功能

<br>

<div grid="~ cols-2 gap-6" m="-t-2">

```c {all|9|11-14}
/* include/fs.h */
#define FSREQ_OPEN  1
#define FSREQ_MAP   2
#define FSREQ_SET_SIZE  3
#define FSREQ_CLOSE 4
#define FSREQ_DIRTY 5
#define FSREQ_REMOVE    6
#define FSREQ_SYNC  7
#define FSREQ_CREATE 8

struct Fsreq_create {
    u_char req_path[MAXPATHLEN];
    u_int req_type;
};
	
	
```

```c{all|2|16}
/* user/fsipc.c */
int fsipc_create(const char *path, u_int type) {
    struct Fsreq_create *req;
	req = (struct Fsreq_create*) fsipcbuf;

	if (strlen(path) >= MAXPATHLEN) {
    	return -E_BAD_PATH;
	}

	strcpy((char*)req->req_path, path);
	req->req_type = type;
	return fsipc(FSREQ_CREATE, req, 0, 0);
}

/* user/lib.h */
int fsipc_create(const char*, u_int);
```

</div>
---

<div grid="~ cols-2 gap-6" m="-t-2">

```c
/* fs/fs.c */
int file_create(char *path, struct File **file) {
    int r;
    /* check if it is a directory */
    char *p1 = (path[0] == '/') ? (path + 1) : 0;
    while (p1 && *p1) {
        while (*p1 && *p1 != '/') {
            p1++;
        }
        if (!*p1 || !*(p1 + 1)) {
            break;
        }
        *p1 = '\0';

        struct File *dir;
        r = self_file_create(path, &dir);
        if (r < 0 && r != -E_FILE_EXISTS) {
            return r;
        }

        *p1++ = '/';
        dir->f_type = FTYPE_DIR;
    }
    return self_file_create(path, file);
}
```

```c{all|3|11|21}
/* fs/serv.c */
void
serve_create(u_int envid, struct Fsreq_create *rq) {
        u_char path[MAXPATHLEN];
        struct File *file;
        int r;

        user_bcopy(rq->req_path, path, MAXPATHLEN);
        path[MAXPATHLEN - 1] = 0;

        r = file_create(path, &file);
        file->f_size = 0;
        file->f_type = rq->req_type;
        ipc_send(envid, r, 0, 0);
}

void
serve(void)
{
    /* ...... */
    case FSREQ_CREATE:
    		serve_create(whom, (struct Fsreq_create *)REQVA);
    break;
}
```

</div>
---



<br>

<br>

<br>

至此，我们可以实现常规文件和目录文件的创建功能，只需要限制文件的 type 即可。

```c
/* user/file.c */
int
create(const char *path, int type)
{
    int r;
    if ((r = fsipc_create(path, type)) < 0) {
        return r;
    }
    return 0;
}
```

---

# **前置工作**

<br>

### **完善文件系统功能**

<br>

- 实现 O_APPEND

```c{all|2|13-15|11}
/* user/lib.h */
#define O_APPEND    0x1000

/* user/file.c */
int
open(const char *path, int mode)
{
	/* ...... */
	va = fd2data(fd);
    ffd = (struct Filefd *)fd;
    size = ffd->f_file.f_size;
    /* ...... */
    if (mode & O_APPEND) {
        seek(fdnum, size);
    }
    return fdnum;
}
```



---

# **清除多余输出**
Make the shell prettier.
<br>

- **关于进程和系统服务的调试信息**
	
	- 删除 spawn, serve_open, env_* 中的输出调试信息
	
	    ![Local Image](/cleanTheOutputs.png)
	
- **清除 `fwritef` 和 `writef` 输出的多余空行**
---
layout: image-right
image: https://source.unsplash.com/collection/94734566/1920x1080
---

# **扩充 string.c 库**
支持更丰富的字符串操作.
<br>
<br>
- 添加了 `strcmp`, `strcat`, `strlen`, `strhash` 等函数

- 由于后续为实现环境变量添加的系统调用也有相应的需求，因此需要在**内核**和**用户态**都进行扩充
	- `user/string.c`
	- `lib/env.c`
---
layout: image-right
image: https://source.unsplash.com/collection/94734566/1920x1080
---

# **特殊字符特判**
`&`  + ` ;`  +  `""`
<br>
- 核心问题：解析输入的字符串时的特殊处理
- 需要关注的核心文件：`user/sh.c`
<br>
<br>

<div grid="~ cols-2 gap-3" m="-t-0">

<div>

![Local Image](/testhang.png)

</div>

<div>
<br>
<br>
<br>

![Local Image](/testmulti.png)

![Local Image](/testquote.png)

</div>

</div>
---

# **特殊字符特判**
`&`  + ` ;`  +  `""`
<br>
<div grid="~ cols-2 gap-5">

<div>

- **& 后台运行**
	
	- 为了处理后台运行，设置一个标记变量 flag_hang，若解析到 & 字符则标记其为 1

	- **修改核心**：原有的内核忙等机制。现在父进程不再等待子进程执行完，直接 exit 返回

</div>

<div  m="-t-10">

~~~c {all|4|10|19}
/* user/sh.c */
void runcmd(char *s) {
	/* ...... */
	int fdnum, flag_hang = 0, pid;
	again:
	argc = 0;
	for (; ;) {
		/* ...... */
		case '&' {
			flag_hang = 1;
			break;
		}
	}
	runit:
	/* ...... */
	close_all();
	if (r >= 0) {
		if (debug_) writef("[%08x] WAIT %s %08x\n", env->env_id, argv[0], r);
		if (!flag_hang)	wait(r);
	}
	/* ...... */
}
~~~

</div>

</div>
---

# **特殊字符特判**
`&`  + ` ;`  +  `""`
<br>

- **; 一行多命令**

	- 解析到分号时，在执行完当前指令后再 goto again 执行下一条指令
	<br>
	<br>

    ```c
      case ';':
        if ((pid = fork()) == 0) {
            fd_redirect[0] = fd_redirect[1] = -1;
            goto again;
        } else {
            goto runit;
        }
        break;
    ```

---

# **特殊字符特判**
`&`  + ` ;`  +  `""`
<br>

- **"" 引号支持**

	- 在解析命令之前需要在参数中解析好，因此关注函数 `gettoken`
	- 解析到引号时，需要利用一个 while 循环继续向下读，直到读到与之匹配的引号，将引号包括起来的部分当成一个字符串来解析
		- 注意转义字符
	<br>
	
	```c {all|4-6|all}
	/* user/sh.c */
	if (*s == '\"') {
    	*p1 = ++s;
    	while (*s && !(*s == '\"' && *(s - 1) != '\\')) {
        	++s;
    	}
    	*s++ = 0;
    	*p2 = s;
    	return 'w';
	}
	```

---
layout: image-right
image: https://source.unsplash.com/collection/94734566/1920x1080
---

# **外部命令扩展**
tree, mkdir, touch
<br>
<br>

- 此时需要针对三个命令分别编写用户态程序 `user/~.c`

	
	
	- **tree**：模仿 ls 命令的逻辑遍历当前目录
	
	    
	
	- **mkdir, touch**：利用先前完善的文件系统功能，调用相应的接口创建文件
	
	- **Extra**：为了强化层次感与优化交互体验，提供彩色输出
---

# **外部命令扩展**
tree, mkdir, touch
<br>

<div grid="~ cols-2 gap-10">

<div>

- **tree命令**

	
	
	- **核心功能**：递归地遍历目录下的文件
		
		- 每读到一个文件都将其文件名按照一定格式打印出来，并根据文件此时位处的深度修改打印的缩进值
		
		
		
		- 如果当前文件为目录文件，则递归地调用遍历函数，将该目录下的文件进行打印

</div>

<div  m="-t-30">

![Local Image](/tree.png)

</div>

</div>
---

# **外部命令扩展**
tree, mkdir, touch
<br>

<div grid="~ cols-2 gap-10">

<div>

- **tree命令**

	
	
	- **核心功能**：递归地遍历目录下的文件
		
		
		
		- 每读到一个文件都将其文件名按照一定格式打印出来，并根据文件此时位处的深度修改打印的缩进值
		
		  
		
		- 如果当前文件为目录文件，则递归地调用遍历函数，将该目录下的文件进行打印

</div>

<div  m="-t-10">

```c{all|9|11|16}
/* user/tree.c */
void walk_dir(char *path, int level) {
    int fd, n;
    struct File f;
    char nextDir[MAXPATHLEN] = {0};

    if ((fd = open(path, O_RDONLY)) < 0)
        user_panic("Fail to open %s: %e", path, fd);
    while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
        if (f.f_name[0]) {
            tree_print(f.f_name, level);
            if(f.f_type == FTYPE_DIR) {
                strcpy(nextDir, path);
                strcat(nextDir, "/");
                strcat(nextDir, f.f_name);
                walk_dir(nextDir, level + 1);
            }
        }
    }
}
```

</div>

</div>
---

# **外部命令扩展**
tree, mkdir, touch
<br>

- **mkdir & touch**

	- 直接调用 *前置工作* 中已经完成的文件创建服务接口即可，但需要注意路径解析问题
	- **mkdir**：a, a/b, a/c/d, a
	- **touch**：test1.c, a/test2.c, f/test3.c, test1.c
	
	<br>
	
	<div grid="~ cols-2 gap-10">
	
	
	```c {all|4-8}
	/* user/touch.c*/
	void touch(char *path, char *prefix) {
		/* ...... */
	    if((r = create(curpath, FTYPE_REG)) < 0) {
	    	fwritef(1, "\033[0m\033[1;31mFile %s Already Exists!\033[0m\n", curpath);
	        return;
	    }
	    fwritef(1, "\033[0m\033[1;33mFile %s created successfully!\033[0m\n", curpath);
	}
	```
	
	```c {all|4-8}
	/*user/mkdir.c */
	void mkdir(char *path, char *prefix) {
	    /* ...... */
	    if((r = create(curpath, FTYPE_DIR)) < 0) {
	        fwritef(1, "\033[0m\033[1;31mDirectory %s Already Exists!\033[0m\n", curpath);
	        return;
	    }
	    fwritef(1, "\033[0m\033[1;33mCreated Directory %s successfully!\033[0m\n", curpath);
	}
	```
	
	</div>

---

# **外部命令扩展**
tree, mkdir, touch
<br>

- **mkdir 效果展示**

<div grid="~ cols-2 gap-10">

<div>

![Local Image](/testmkdir1.png)

</div>

<div m="-t-41">

![Local Image](/testmkdir2.png)

</div>

</div>

---

# **外部命令扩展**
tree, mkdir, touch
<br>

- **touch 效果展示**

<div grid="~ cols-2 gap-10">

<div>

![Local Image](/testtouch1.png)

</div>

<div m="-t-41">

![Local Image](/testtouch2.png)

</div>

</div>
---
layout: image-right
image: https://source.unsplash.com/collection/94734566/1920x1080
---

# **内部命令扩展**
history & declare & unset
<br>

#### **history**
<br>

- 核心功能：历史输入命令的记录、导出与按键回溯
![Local Image](/testhistory.png)

---

# **内部命令扩展**
history & declare & unset
<br>

#### **history**
<br>

- **记录**：将在 shell 中输入的每步指令，在解析前保存进一个专用文件 `.history` 中，每行一条指令

	- **history_init**：在解析指令前进行 history_init 时自动创建该文件用以保存历史命令
		- 支持：文件系统中的创建文件服务
		
	
	<br>
	
	<div grid="~ cols-2 gap-10">
	
	
	```c
	/* user/sh.c*/
	void u_main(int argc, char **argv) {
		history_init();
		/* ...... */
	}
	```
	
	```c
	/* user/sh_history.c*/
	void history_init() {
	    int r;
	    if((r = create("/.history", FTYPE_REG)) < 0)
	        user_panic("Fail to initialize /.history: %d.", r);
	}
	```
	
	</div>
---

# **内部命令扩展**
history & declare & unset
<br>

#### **history**
<br>

- **按行保存**

	- 在指令解析之前及时将指令内容写入到 .history 文件的末尾
		- 支持：O_APPEND
	
	<br>
	
	<div grid="~ cols-2 gap-10">
	
	```c
	/* user/sh.c */
	void runcmd(char *s) {
	    history_save(s);
	    /* ...... */
	}
	```
	
	```c
	/* user/sh_history.c */
	void history_store(char *s) {
	    int r;
	    if ((r = open("/.history", O_RDWR | O_CREAT | O_APPEND)) < 0)
	        user_panic("Fail to open /.history.");
	    fwritef(r, "%s\n", s);
	    close(r);
	}
	```
	
	</div>
---

# **内部命令扩展**
history & declare & unset
<br>

#### **history**
<br>

- **按键回溯**

	<div grid="~ cols-2 gap-10">

	<div>

	- 按键对应的指令：`esc[A`, `esc[B`
	- 修改核心：`readline`
		- 将 `.history` 记录的命令保存到一个二维数组里，以实现按行移动

	</div>
	
	<div m="-t-50">
	
	![Local Image](/mindmap.png)
	
	</div>

	</div>
---

# **内部命令扩展**
history & declare & unset
<br>

#### **history**
<br>

- **按键回溯**

	<div grid="~ cols-2 gap-10">

	<div>

	- 按键对应的指令：`esc[A`, `esc[B`
	- 修改核心：`readline`
		- 将 `.history` 记录的命令保存到一个二维数组里，以实现按行移动

	</div>
	
	<div m="-t-45">
	
	```c
	/* user/sh.c_readline */
		if (i >= 2 && buf[i - 2] == 27 && buf[i - 1] == 91 && buf[i] == 65) {	// Up arrow key
	    	// Clean all the words in the current line
      	int cleanLen = strlen(buf) - 3;
	    	writef("%c%c%c", 27, 91, 66);   // Keep the pointer at the bottom
	    	flush(cleanLen);
	
	    	if (cmdi) 
	        	strcpy(buf, cmds[--cmdi]);
	    	else 
	        	strcpy(buf, cmds[cmdi]);
	    	writef("%s", buf);
	    	i = strlen(buf) - 1;    // goto end
		} else if (i >= 2 && buf[i - 2] == 27 && buf[i - 1] == 91 && buf[i] == 66) { 	// Down arrow key
	    	int cleanLen = strlen(buf) - 3;
	    	flush(cleanLen);
	
	    	if (cmdi < cmdn - 1) {
	        	strcpy(buf, cmds[++cmdi]);
	        	writef("%s", buf);
	    	}
	    	i = strlen(buf) - 1;    //goto end
	}
	```
	
	</div>
	
	</div>
---

# **内部命令扩展**
history & declare & unset
<br>

#### **Challenge:	环境变量**
<br>

<div grid="~ cols-2 gap-10">

<div>

- **原理**

	- NAME + VALUE：构造哈希表进行映射对照
	- 应当存储在**内核态**，供给用户态程序共享读取
		- 构造对应的系统调用 syscall_envar 进行调用

</div>

<div>

- **步骤**

	- 构建环境变量系统调用 syscall_envar，沟通用户态和内核态
	- 添加对应的命令文件 `declare.c`, `unset.c`
	- 设置一个数组 `definedAge[]` 记录当前环境，以区分局部变量与环境变量
	- echo \${VARIABLE}：在 echo.c 中解析 `$` 开头的字符串进行替换
</div>

</div>

---

# **内部命令扩展**
history & declare & unset
<br>

#### **Challenge:	环境变量**
<br>

- **syscall_envar**

	- `user/syscall_lib.c`:		int syscall_envar(char *name, char *value, u_int op, u_int isEnvar)
	- `lib/syscall_all.c`:		  int sys_envar(int sysno, char *name, char *value, u_int op, u_int isEnvar)
	- `lib/syscall.S`:			   .word sys_envar
	- `include/unistd.h`:	  #define SYS_envar
---

# **内部命令扩展**
history & declare & unset
<br>

#### **Challenge:	环境变量**
<br>
<br>

- **syscall_envar**


<div grid="~ cols-2 gap-5">

<div>

<br>

int sys_envar(int sysno, char *name, char *value, u_int op, u_int isEnvar)

</div>

<div m="-t-30">

```c
int sys_envar(int sysno, char *name, char *value, u_int op, u_int isEnvar) {
    const int MOD = 256;
    static char name_table[256][64];
    static char value_table[256][256];
    static int is_readonly[256];
    static int definedAge[256];	/* When the variable is declared, the age is ... */
    static int currentAge;
    /* If definedAge == -1, then it is an environment variable; */
    /* Else if defineAge == currentAge, then it is a local variable. */
    
    if (strcmp(name, "curpath") == 0) {
		currentAge++;		
        /* Every time we invoke the command 'sh' to create a shell program, we do this. */
    }
    
    /* [1] Find the position of the NAME in our hashmap. */
    /* [2] Operate according to the value of op. */
    
    return 0;
}
```

</div>

</div>

---

<br>
<br>

<div grid="~ cols-3 gap-5">

<div>

```markdown{1-13|15-19}
-------[op]-----------

0	创建变量 NAME-VALUE

1	获得 NAME 的 VALUE 值

2	重新定义变量 NAME

3	删除非只读的变量 NAME

4	输出当前进程的所有变量

5	设置只读变量 NAME-VALUE

-------[isEnvar]-------

1	environment variable

0	local variable
```

</div>

<div m="-t-10">

```c
/* [2] */
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
    } 

```

</div>

<div>

```c
else if (op == 2) {
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
    } 
```

</div>

</div>

---

<br>
<br>

<div grid="~ cols-2 gap-10">

<div>

```c
else if (op == 3) {
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
    }
```

</div>

<div m="-t-10">

```c
else if (op == 4) {
		// List all the variable
        int pos = 0, i, j, cnt = 0;
		for (i = 0; i < 256; ++i)
			if (name_table[i][0]) {
				if (strcmp(name_table[i], "curpath") == 0) {
					continue;	
				}
            	printf("NAME: %s", name_table[i]);
				for (j = 0; j < (16 - strlen(name_table[i])); j++)	printf(" ");
				printf("VALUE: %s", value_table[i]);
				for (j = 0; j < (16 - strlen(value_table[i])); j++)	printf(" ");
				if (definedAge[i] == -1) {	printf("\033[0m\033[1;34m--[ENVIRONMENT VARIABLE]\033[0m");	}
				else {	printf("\033[0m\033[1;32m--[LOCAL VARIABLE]\033[0m");	}
				if (is_readonly[i]) {	printf("\033[0m\033[1;35m--[READ_ONLY]\033[0m");	}
				printf("\n");
				cnt++;
            }
		if (cnt == 0) {	printf("\033[0m\033[1;33mHaven't declare any variable in this shell now.\n\033[0m");   }
}
else if (op == 5) {
		// Set an variable as read-only
		is_readonly[pos] = 1;
}
```

</div>

</div>

---

# **测试工作**
Test is important.
<br>
<br>

<div grid="~ cols-2 gap-5">

<div>

- 命令组合检查 shell 输出现象

- 编写测试程序测试后台命令

</div>

<div m="-t-10">

```c
/* user/testhang.c */
void testHang(int n) {
    int i = 2;
    int checkpoint = n / 10;
    for (; i <= n; i++) {
        if (i % checkpoint == 0) {
            writef("\033[0m\033[1;32m@@@ Hey it is hanging @@@\n\033[0m");
        }
        if (isprime(i)) ;
    }
}

void umain() {
    writef("\033[0m\033[1;31m--- Start to test a slow nohup program ---\n\033[0m");
    testHang(100000);
    writef("\033[0m\033[1;31m--- The nohup progame has done! ---\n\033[0m");
    return;
}
```

</div>

</div>
---

# **问题及解决方案**
Problems & solutions.
<br>
<br>

- **方向键对应的指令**

	- 命令行窗口调试
- **上下键回溯命令时如何使得 heading 不被错误覆盖**
	- 弄明白 buf 存储的内容和 `readline` 的流程，以正确计算退格清空字符的个数
- **区别局部变量和环境变量**
	- 定义 `definedAge[]` 和 `currentAge`
	- 每次调用 `sh` 时调用一次 syscall_envar 增加当前 currentAge，据此区分父 shell 和子 shell
	- echo 调用 sys_envar 替换变量值时比较每个变量的 `definedAge` 和 `currentAge`

---

# **Extra**
Extra works that I have done.

<br>

- `.b` 补全

	- runcmd 进入 `spawn` 前为 argv[0] 补全 `.b` 字符
- 部分语句的彩色输出
	
	- ```c
	    printf("\033[0m\033[1;%dm%s\033[0m");
	    // %d - Indicate the color number
	    // %s - The string that you want to print
	    ```
	
	    
---
layout: center
class: text-center
---

# Complete Code and Report

[Github](https://github.com/GwokHiujin/BUAA_OS_2022)