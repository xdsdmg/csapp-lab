# Arch Lab

题解参考：

- <https://zhuanlan.zhihu.com/p/641239498>
- <https://zhuanlan.zhihu.com/p/77072339>
- <https://lincx-911.github.io/2021/12/architecture_lab/>

## 前置条件

### 修改 Makefile

修改 misc、pipe、seq、y86-code 的 Makefile，参考：<https://stackoverflow.com/questions/63152352/fail-to-compile-the-y86-simulatur-csapp>

### 安装依赖

``` bash
sudo apt install flex bison tcl tcl-dev tk tk-dev
```

## Part A

- [sum_list](./sim/misc/sum.ys)
- [rsum_list](./sim/misc/rsum.ys)
- [copy_block](./sim/misc/copy.ys)

## Part B

在执行 `make VERSION=full` 时会报错，执行下面命令，然后注释掉 ssim.c 中涉及 matherr 两行代码（[参考链接](https://lincx-911.github.io/2021/12/architecture_lab/)）。

``` shell
sed -i "s/tcl8.5/tcl8.6/g" Makefile
sed -i "s/CFLAGS=/CFLAGS=-DUSE_INTERP_RESULT /g" Makefile 
```

题解：[seq-full.hcl](./sim/seq/seq-full.hcl)

## Part C

对于下面报警可通过以下命令修复

``` plain
perl: warning: Setting locale failed.
perl: warning: Please check that your locale settings:
  LANGUAGE = (unset),
  LC_ALL = "en_US.UTF-8",
  LC_CTYPE = "en_US.UTF-8",
  LC_TERMINAL = "iTerm2",
  LANG = "C"
    are supported and installed on your system.
```

``` shell
sudo apt-get update 
sudo apt-get install -y locales   
locale-gen en_US.UTF-8
```

主要思路是使用循环展开的方法消除内存复制过程中的数据冒险。

题解：[ncopy.ys](./sim/pipe/ncopy.ys)、[pipe-full.hcl](./sim/pipe/pipe-full.hcl)

可以达到如下分数：

``` plain
Average CPE 8.42
Score 41.5/60.0
```

可以通过 `make build && make test` 命令测试题解。

如果再想提高可以考虑优化剩余元素处理的部分，引入二分查找策略，具体可参考最上方的连接。
