执行方法：

```bash
gc.exe 10 123456 < 1.txt > ans1.txt
```

`10`表示执行时间为10s，`123456`表示输入种子为123456，`1.txt`为输入算例，`ans.txt`为输出算例。

批量化测试脚本

```bash
@echo off
setlocal enabledelayedexpansion

:: 运行次数
set REPEAT_COUNT=5

:: 创建主目录ans文件夹（如果不存在）
if not exist ans (
    mkdir ans
)

:: 遍历1.txt到9.txt，执行每个文件
for /L %%i in (1,1,5) do (
    echo Running example %%i...

    :: 创建子目录ans<算例编号>（如ans1、ans2等）
    if not exist ans\ans%%i (
        mkdir ans\ans%%i
    )

    :: 创建并发任务，执行每个算例 REPEAT_COUNT 次
    for /L %%j in (1,1,%REPEAT_COUNT%) do (
        echo Run %%j for example %%i
        start /B gc.exe 10000 1 < %%i.txt > ans\ans%%i\ans_%%i_%%j.txt
    )
)

pause

echo All executions started.
endlocal

```

