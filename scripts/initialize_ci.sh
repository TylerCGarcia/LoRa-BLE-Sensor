pip install west

west init -l zephyr
 # west update --narrow -o=--depth=1 
west update

west packages pip --install