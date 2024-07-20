#ifndef KERNEL_H
#define KERNEL_H

class Kernel {
    public:
        Kernel();
        static float poly6Kernel(float r, float h);
        static float smoothKernel(float r, float h);
        static float gaussianKernel(float r, float h);
};

#endif