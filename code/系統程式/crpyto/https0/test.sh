set -x
make clean
make 
./httpd_ssl0 8443 cert.pem key.pem