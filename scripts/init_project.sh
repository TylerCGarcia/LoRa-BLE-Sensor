sudo apt install python3-venv

python3 -m venv .venv

source .venv/bin/activate

pip install west

west init -l zephyr

west update

west packages pip --install