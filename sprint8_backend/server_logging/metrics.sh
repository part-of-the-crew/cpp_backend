#!/usr/bin/env bash
set -e
#wget https://github.com/prometheus/node_exporter/releases/download/v1.4.0/node_exporter-1.4.0.linux-amd64.tar.gz
#tar xvzf node_exporter-1.4.0.linux-amd64.tar.gz
#cd node_exporter-1.4.0.linux-amd64/
##~/node_exporter-1.4.0.linux-amd64/node_exporter &
#http://<ip>:9100/
#nohup ./node_exporter & 
#sudo ufw allow 9100/tcp

#sudo usermod -aG docker $USER
docker start prom 2>/dev/null || \
docker run -d \
  --name prom \
  --network=host \
  -v "$(pwd)/prometheus.yml:/etc/prometheus/prometheus.yml:ro" \
  prom/prometheus \
  --config.file=/etc/prometheus/prometheus.yml
#docker run -d --name prom --network="host" prom/prometheus
#sudo ufw allow 9090/tcp
#sudo docker cp prometheus.yml prom:/etc/prometheus
#sudo docker restart prom
#avg(1 - rate(node_cpu_seconds_total{mode="idle"}[1m])) * 100
#(1 - node_memory_MemFree_bytes / node_memory_MemTotal_bytes) * 100
#(1 - node_filesystem_free_bytes{device="/dev/vda2"} / node_filesystem_size_bytes{device="/dev/vda2"}) * 100
docker start grafana 2>/dev/null || \
docker run -d --name=grafana --network="host" grafana/grafana