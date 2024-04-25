本LCSB测试也是基于C++编程，使用C++IDE进入后直接点击编译按钮，然后根据输出提示依次输入工作负载类型以及测试树的类型即可/
（测试树类型1对应Compressed Radix Tree，2对应Radix tree，3对应红黑树。）
耐心等待一段时间后会输出LCSB测试结果，average后是平均时延。half是中位数，ninety percent是90%的数据的时延，ninety-nine是99%的数据的时延

Tips：测试compressed radix tree可能会有些卡顿。测试时间可能比较长，因为时延较长，估计会到1-3分钟//