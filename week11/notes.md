# Работа с процеси в C (05.05.2026)

## 1. Семейството exec() функции

### Теория

Функциите от семейството `exec()` заменят текущия процес с нова програма. След успешно изпълнение, кодът на текущия процес се заменя изцяло - exec **не се връща** при успех. Тоест можем да си представим, че кодът "надолу се заменя изцяло" с изпълнението на командата в `exec()`.

| Функция | Описание |
|---------|----------|
| `execl()` | Аргументи като списък, търси в конкретен път |
| `execlp()` | Аргументи като списък, търси в PATH |
| `execle()` | Аргументи като списък + environment |
| `execv()` | Аргументи като масив (vector) |
| `execvp()` | Аргументи като масив, търси в PATH |
| `execve()` | Системно извикване - аргументи + environment |

**Конвенция за именуване:**
- `l` = list (аргументи като списък)
- `v` = vector (аргументи като масив)
- `p` = PATH (търси изпълнимия файл в PATH)
- `e` = environment (позволява задаване на environment)

### Практика

```c
int main() {
    // execl - пълен път, аргументи като списък
    execl("/bin/ls", "ls", "-l", "-a", (char*)NULL);
    
    // execlp - търси в PATH
    execlp("ls", "ls", "-l", (char*)NULL);
    
    // execv - аргументи като масив
    char *args[] = {"ls", "-l", "-a", (char*)NULL};
    execv("/bin/ls", args);
    
    // execvp - масив + търсене в PATH
    execvp("ls", args);
    
    // execle - със собствен environment
    char *env[] = {"HOME=/tmp", "USER=test", (char*)NULL};
    execle("/bin/ls", "ls", "-l", (char*)NULL, env);
    
    // Ако стигнем тук - exec не е успял да се изпълни коректно, затова няма нужда и от проверката с if, може и просто с err на следващия ред.
    err(1,"exec failed");
}
```

---

## 2. fork() - Създаване на процес

### Теория

`fork()` създава копие на текущия (родителски) процес:

```
           fork
Parent ─────┬────→ Parent връща PID-а на детето
            │
            └────→ Child  връща 0
```

**Copy-on-Write (CoW):**
- Семантично паметта се копира
- Реално страниците в паметта са read-only и се споделят, избягва ненужно копиране
- При опит за писане (променяне на нещо), kernel копира само променената страница

**Какво наследява детето:**
- UID, EUID, GID
- Environment променливи
- Отворени файлови дескриптори
- Текущата директория
- umask

### Практика

```c
int main() {
    int x = 10;
    int pid = fork();
    if (pid < 0) {
        err(1,"fork failed");
    }
    
    if (pid == 0) {
        // Child
        x = 20;  // CoW - копира се "страницата"
        printf("Child: x=%d\n", x);
    } else {
        // Parent
        printf("Parent: x=%d\n", x);  // x e 10
    }
    
    return 0;
}
```

**Типичен fork() + exec() пример:**

```c
int main() {
    int pid = fork();
    
    if (pid == 0) {
        // Child
        execlp("ls", "ls", "-la", NULL);
        err(1, "exec failed");  // Само ако exec fail-не
    }
    
    int status;
    wait(&status);
    
    return 0;
}
```

---

## 3. wait() и waitpid() - Изчакване на дете

### Теория

**wait():**
- Блокира докато **някое** дете завърши
- Връща PID на завършилото дете
- Записва статуса в подадения указател

**waitpid():**
- Може да чака **конкретно** дете (по PID)

**Някои от макротата за статус:**

| Макро | Какво прави |
|-------|-------------|
| `WIFEXITED(status)` | Дали процесът завърши нормално |
| `WEXITSTATUS(status)` | Exit code (0-255), валидно само ако WIFEXITED |
| `WIFSIGNALED(status)` | Дали процесът беше убит от сигнал |
| `WIFSTOPPED(status)` | Дали процесът е спрян |

### Практика

```c
int main() {
    int pid = fork();
    
    if (pid == 0) {
        // Child
        sleep(10);
        exit(42);
    }
    
    // Parent
    int status;
    int finished = wait(&status);
    
    if (!WIFEXITED(status)) {
        err(1,"didn't exit normally");
    } else if (WIFSIGNALED(status)) {
        err(2,"killed");
    }
    
    return 0;
}
```


**Bonus example: Изчакване на конкретно дете:**

```c
// Създаваме няколко деца
pid_t child1 = fork();
if (child1 == 0) { sleep(10); exit(1); }

pid_t child2 = fork();
if (child2 == 0) { sleep(15); exit(2); }

// Чакаме само child2
int status;
waitpid(child2, &status, 0);  // Блокира докато child2 завърши

// child1 може да е zombie докато не го изчакаме и хванем
waitpid(child1, &status, 0);
```

---

## 4. kill() - Изпращане на сигнали

### Теория

```c
int kill(pid_t pid, int sig);
```

**Стойности на pid:**

За положителни стойности изпраща до процес с този PID, а за -1 изпраща до всички процеси (с права). Тук може да се използват вече известните макрота за сигнали (SIGTERM, SIGKILL, SIGINT, etc.)

### Практика

```c
int main() {
    int pid = fork();
    
    if (pid == 0) {
        // Child
        while (1) {
            sleep(1);
        }
    }
    
    // Parent
    sleep(5);
    kill(pid, SIGTERM);
    
    int status;
    wait(&status);
    
    if (WIFSIGNALED(status)) {
        err(1,"Child killed by signal");
    }
    
    return 0;
}
```

---

## 6. Често срещани грешки

| Грешка | Последствие | Решение |
|--------|-------------|---------|
| Не се чака дете | Zombie процеси | В голяма част от случаите ще е необходимо да викате wait() |
| fork() в цикъл | Fork bomb | Ограничете броя fork() извиквания |
| Споделяне на fd след fork() | Race conditions | Затваряйте неизползвани fd, но за това в следващото упражнение ще говорим повече | 

---

## 7. Задачи
От репото

---
## Helper functions as promised

```c
int safe_write(int fd, const void* elem, size_t size) {
    int bytes = write(fd, elem, size);
    if (bytes < 0) {
        err(2, "write to %d failed", fd);
    }
    return bytes;
}

int safe_read(int fd, void* elem, size_t size) {
    int bytes = read(fd, elem, size);
    if (bytes < 0) {
        err(3, "read from %d failed", fd);
    }
    return bytes;
}

int safe_wait(void) {
    int status;
    if (wait(&status) < 0) {
        err(4, "couldn't wait for a child");
    }

    if (!WIFEXITED(status)) {
        errx(5, "child couldn't exit normally"); // killed by a signal, etc.
    }
    
    return WEXITSTATUS(status);
}
```
