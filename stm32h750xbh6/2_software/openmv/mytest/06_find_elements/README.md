霍夫变换(hough): https://en.wikipedia.org/wiki/Circle_Hough_Transform

img.find_circles()

- `threshold`: 控制找到的圆数量, 数值越大检测到的圆越少

- `x_margin`, `y_margin`, `r_margin`：控制相近圆的合并

- `r_min`, `r_max`, `r_step`：控制测试圆的半径

img.find_line_segments()

- `merge_distance`: 控制相近线段的合并, 单位位像素, 0 表示不合并

- `max_theta_diff`: 控制相差一定角度的线段合并, 单位为度

img.find_lines()

- `threshold`: 控制找到的直线数量, 数值越大检测到的圆越少

- `theta_margin`、`rho_margin`: 控制相差一定角度的直线合并
