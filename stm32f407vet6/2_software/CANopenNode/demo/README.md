[CANopenNode](https://github.com/CANopenNode/CANopenNode) 

[canfestival](https://canfestival.org/index.html.en) 

* 对象字典

![1](.assest/README/1.png)

* 中断配置

![2](.assest/README/2.png)

**CiA 301 - CANopen application layer and communication profile**

- Unlimited number of SDO servers which supports:
  - Expedited transfers
  - Segmented transfers
  - Block transfers
- Unlimited number of SDO clients which supports:
  - Expedited transfers
  - Segmented transfers
- Unlimited number of TPDO and RPDOs which supports:
  - Synchronized operation
  - Asynchronous operation
  - Manufacturer specific operation
- Unlimited number of entries in object dictionary
  - Static or dynamic object dictionary
  - Data types: signed/unsigned 8/16/32-Bit integer, string, domain and user-types
- Unlimited number of parameter groups for parameter storage
- Emergency producer which supports:
  - Manufacturer specific extensions
  - Unlimited error history
- Unlimited Emergency consumers
- SYNC producer
- Unlimited number of SYNC consumers
- Heartbeat producer
- Unlimited number of Heartbeat consumers
- Network Management consumer

**CiA 305 - Layer Setting Services (LSS)**

- Baudrate configuration
- NodeId configuration

*Note: the term 'unlimited' means, that the implementation introduces no additional limit. There are technical limits, described in the specification (for example: up to 511 possible TPDOs)*