#FROM nvidia/cuda:10.0-cudnn7-devel-ubuntu18.04
FROM nvidia/cudagl:10.0-devel-ubuntu18.04

USER root

ENV DEBIAN_FRONTEND noninteractive

WORKDIR /tmp

RUN apt update

RUN apt -y install git cmake gcc g++ htop \
    build-essential libxmu-dev libxi-dev liblapack-dev \
    libgl-dev libglew-dev libglew2.0 liblapack-dev freeglut3-dev \
    libatlas-base-dev python python-pip libcgal-dev \
    libglm-dev libboost-filesystem-dev libgflags-dev \
    libgflags2.2 libgoogle-glog-dev libgoogle-glog0v5 \
    protobuf-compiler libprotobuf-dev libhdf5-dev liblmdb-dev \
    libleveldb-dev libsnappy-dev libboost-python-dev wget curl \
    doxygen

RUN apt-get update && apt-get install -y --no-install-recommends gnupg2 curl ca-certificates && \
    curl -fsSL https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/7fa2af80.pub | apt-key add - && \
    echo "deb https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64 /" > /etc/apt/sources.list.d/cuda.list && \
    echo "deb https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64 /" > /etc/apt/sources.list.d/nvidia-ml.list && \
    apt-get purge --autoremove -y curl && \
    rm -rf /var/lib/apt/lists/*

ENV CUDA_VERSION 10.0.130
ENV CUDA_PKG_VERSION 10-0=$CUDA_VERSION-1

# For libraries in the cuda-compat-* package: https://docs.nvidia.com/cuda/eula/index.html#attachment-a
RUN apt-get update && apt-get install -y --no-install-recommends \
        cuda-cudart-$CUDA_PKG_VERSION \
        cuda-compat-10-0 && \
    ln -s cuda-10.0 /usr/local/cuda && \
    rm -rf /var/lib/apt/lists/*

# Required for nvidia-docker v1
RUN echo "/usr/local/nvidia/lib" >> /etc/ld.so.conf.d/nvidia.conf && \
    echo "/usr/local/nvidia/lib64" >> /etc/ld.so.conf.d/nvidia.conf

ENV PATH /usr/local/nvidia/bin:/usr/local/cuda/bin:${PATH}
ENV LD_LIBRARY_PATH /usr/local/nvidia/lib:/usr/local/nvidia/lib64

# nvidia-container-runtime
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility,graphics,video
ENV NVIDIA_REQUIRE_CUDA "cuda>=10.0 brand=tesla,driver>=384,driver<385 brand=tesla,driver>=410,driver<411"

ENV NCCL_VERSION 2.4.2

RUN apt-get update && apt-get install -y --no-install-recommends \
        cuda-libraries-$CUDA_PKG_VERSION \
        cuda-nvtx-$CUDA_PKG_VERSION \
        libnccl2=$NCCL_VERSION-1+cuda10.0 && \
    apt-mark hold libnccl2 && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
        cuda-libraries-dev-$CUDA_PKG_VERSION \
        cuda-nvml-dev-$CUDA_PKG_VERSION \
        cuda-minimal-build-$CUDA_PKG_VERSION \
        cuda-command-line-tools-$CUDA_PKG_VERSION \
        libnccl-dev=$NCCL_VERSION-1+cuda10.0 && \
    rm -rf /var/lib/apt/lists/*

ENV LIBRARY_PATH /usr/local/cuda/lib64/stubs
ENV CUDNN_VERSION 7.6.0.64

RUN apt-get update && apt-get install -y --no-install-recommends \
            libcudnn7=$CUDNN_VERSION-1+cuda10.0 \
            libcudnn7-dev=$CUDNN_VERSION-1+cuda10.0 && \
    apt-mark hold libcudnn7 && \
    rm -rf /var/lib/apt/lists/*

RUN wget https://github.com/Kitware/CMake/releases/download/v3.14.3/cmake-3.14.3.tar.gz

RUN tar -xf cmake-3.14.3.tar.gz

WORKDIR /tmp/cmake-3.14.3

RUN mkdir build

WORKDIR /tmp/cmake-3.14.3/build

RUN cmake .. && make install -j4

RUN hash -r

WORKDIR /tmp

RUN rm -rf /tmp/.X11-unix/ /tmp/.X1-lock

RUN git clone https://github.com/davisking/dlib.git

WORKDIR /tmp/dlib

RUN mkdir build

WORKDIR /tmp/dlib/build

RUN cmake ..

RUN make install -j4

WORKDIR /tmp

RUN git clone https://github.com/g-truc/glm.git

WORKDIR /tmp/glm

RUN mkdir build

WORKDIR /tmp/glm/build

RUN cmake ..

RUN make install -j4

WORKDIR /tmp

RUN git clone https://github.com/BVLC/caffe.git

RUN apt update && apt install -y libopencv-dev python-numpy qtcreator qt5-default qt5-image-formats-plugins

WORKDIR /tmp/caffe

#RUN cd python && for req in $(cat requirements.txt) pydot; do pip install $req; done && cd .. && \
#    git clone https://github.com/NVIDIA/nccl.git && cd nccl && make -j4 install NVCC_GENCODE="-gencode=arch=compute_37,code=sm_37" && cd .. && rm -rf nccl && \
RUN   mkdir build && cd build

WORKDIR /tmp/caffe/build

ENV CUDA_ARCH_BIN "37"
ENV CUDA_ARCH_PTX "37"
RUN cmake -DCUDA_ARCH_NAME=Manual -DCUDA_ARCH_BIN="${CUDA_ARCH_BIN}" -DCUDA_ARCH_PTX="${CUDA_ARCH_PTX}" ..

RUN make install -j4

WORKDIR /tmp/caffe

RUN protoc src/caffe/proto/caffe.proto --cpp_out=. && \
    mkdir include/caffe/proto && \
    mv src/caffe/proto/caffe.pb.h include/caffe/proto

ENV LD_LIBRARY_PATH=/tmp/caffe/build/lib:$LD_LIBRARY_PATH

WORKDIR /srv/app/
RUN mkdir -p /srv/app/photo2bldg
COPY server /srv/app/photo2bldg/server
COPY src /srv/app/photo2bldg/src
COPY CMakeLists.txt /srv/app/photo2bldg
COPY extras/FindEGL.cmake /usr/local/share/cmake-3.14/Modules
#RUN git clone https://github.com/may5694/photo2bldg.git
WORKDIR /srv/app/photo2bldg
RUN mkdir -p build
WORKDIR /srv/app/photo2bldg/build
RUN cmake ..
RUN make -j4


#CMD ["/usr/bin/vncserver", "-fg"]
