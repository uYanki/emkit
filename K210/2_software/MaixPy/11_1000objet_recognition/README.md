1.把`mobilenet.kmodel`和`labels.txt`放到 TF 卡中。

2.因为模型有 4.2MiB，所以需要烧录带`minimum`固件。并且通过命令行执行以下代码：

```python
from Maix import utils
import machine
utils.gc_heap_size(256*1024)
machine.reset()
```
