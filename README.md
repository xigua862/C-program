# C-Isaac（暂名）

用纯 C + [raylib](https://www.raylib.com/) 复刻《以撒的结合》核心玩法的 roguelike 学习项目。

## 开发环境

- Windows 10/11
- [w64devkit](https://github.com/skeeto/w64devkit)（GCC + GNU make，绿色便携）
- raylib 6.0（`vendor/` 目录自带预编译库，clone 即可编译，无需另行下载）

## 构建与运行

```bat
build.bat        rem 编译
build.bat run    rem 编译并运行
build.bat clean  rem 清理产物
```

## 操作

- WASD：移动
- 方向键：射击眼泪
- R：死亡后重开

## 里程碑

- [x] M0 工具链跑通（窗口 + 移动方块）
- [x] M1 核心切片：移动 + 四向射击 + 追踪怪 + HP + 死亡重开
- [ ] M2 房间系统：清怪开门、房间切换、门连接
- [ ] M3 楼层程序生成：房间网格、随机布局、Boss 房 / 宝箱房 / 楼梯
- [ ] M4 道具系统：道具池、属性修改、协同效果
- [ ] M5 Boss 战 + 多种怪物 AI
- [ ] M6 打磨：HUD、音效、粒子、Meta 解锁

## 目录结构

```
src/      游戏源码
vendor/   raylib 预编译库（随仓库提交，任何机器 clone 即可编译）
assets/   美术 / 音频资源（后续加入）
```
