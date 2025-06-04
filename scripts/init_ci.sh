apt install python3-venv

python3 -m venv .venv

. .venv/bin/activate

pip install west

west init -l zephyr

west update --narrow -o=--depth=1 

west packages pip --install