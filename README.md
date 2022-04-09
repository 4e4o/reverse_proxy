git clone --recurse-submodules git@github.com:4e4o/reverse_proxy.git  
mkdir build_reverse_proxy && cd build_reverse_proxy  
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../reverse_proxy/ .  
cmake --build .  
sudo cmake --install .  

sudo systemctl daemon-reload  
sudo systemctl enable reverse_proxy.service  
sudo systemctl start reverse_proxy.service  
