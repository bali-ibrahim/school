sudo apt-get install rpcbind
sudo /etc/init.d/rpcbind start
make -f Makefile.calculator
./calculator_server
# ./calculator_client localhost
