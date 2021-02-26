docker-compose up -d
docker exec -it influxdb influx
CREATE DATABASE sensors
CREATE USER telegraf WITH PASSWORD 'telegraf'
GRANT ALL ON sensors TO telegraf
exit
