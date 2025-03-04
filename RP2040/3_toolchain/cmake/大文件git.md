大文件 git：[Git LFS](https://git-lfs.com/)（Large File Storage）

```shell
# 启用大文件管理
$ git lfs install
# 添加需要管理的大文件
$ git lfs track "pico-sdk.zip"
# 将文件添加到 git 仓库
$ git add "pico-sdk.zip"
$ git commit -m "add pico-sdk"
$ git push

# 移动到上层目录
$ git mv "pico-sdk.zip" ../
$ git commit -m "mv pico-sdk"
$ git push
```

