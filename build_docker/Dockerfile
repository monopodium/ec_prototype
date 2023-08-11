FROM centos:7 AS env

RUN yum update -y &&\
    yum install -y centos-release-scl &&\
    yum install -y devtoolset-9 &&\
    echo "source /opt/rh/devtoolset-9/enable" >> /etc/bashrc
SHELL ["/bin/bash", "--login", "-c"]

RUN yum install -y wget \
    git \
    libtool \
    autoconf \
    automake \
    libevent-devel \
    libevent-2.0.21 \
    zlib \
    zlib-devel \
    openssl \
    openssl-devel \
    openssh-server \
    vim \
    telnet; \
    yum install libffi-devel  \
    perl make which epel-release -y ;\
    yum install -y openssl11-devel 
    

RUN mkdir -p /tmp/cmake && \
    pushd /tmp/cmake && \
    wget 'https://cmake.org/files/v3.22/cmake-3.22.1-linux-x86_64.sh' && \
    bash cmake-3.22.1-linux-x86_64.sh --prefix=/usr/local --exclude-subdir && \
    popd && \
    rm -rf /tmp/cmake

WORKDIR /home
ADD ./third_party /home/pkg_third_party
ADD install_third_party.sh /home/

RUN chmod +x ./install_third_party.sh &&\
    /bin/sh ./install_third_party.sh

RUN yum install -y zsh; \
    git clone  https://ghproxy.com/https://github.com/ohmyzsh/ohmyzsh.git ~/.oh-my-zsh; \
    cp ~/.oh-my-zsh/templates/zshrc.zsh-template ~/.zshrc; \
    sed -i "s/robbyrussell/ys/" ~/.zshrc; \
    git clone  https://ghproxy.com/https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions; \
    git clone  https://ghproxy.com/https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting; \
    sed -i "s/plugins=(git.*)$/plugins=(git zsh-syntax-highlighting zsh-autosuggestions)/" ~/.zshrc 

# # ENTRYPOINT ["zsh"]

RUN curl https://www.python.org/ftp/python/3.10.0/Python-3.10.0.tgz --output /tmp/Python-3.10.0.tgz;\
    cd /tmp ;\
    tar xzf Python-3.10.0.tgz;\
    cd /tmp/Python-3.10.0; \
    sed -i 's/PKG_CONFIG openssl /PKG_CONFIG openssl11 /g' configure; \
    ./configure --enable-optimizations; \
    make altinstall; \
    rm -r /tmp/Python-3.10.0.tgz;\
    cd /tmp;\
    curl https://bootstrap.pypa.io/get-pip.py --output get-pip.py;\
    python3.10 get-pip.py;\
    python3.10 -m pip install --upgrade pip
# EXPOSE 11211
# ENTRYPOINT /home/memcached/bin/memcached
# CMD ["-m", "1024","--max-item-size=16777216", "-u","root"]



  