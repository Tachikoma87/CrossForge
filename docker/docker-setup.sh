function install_docker () {
	# uninstall conflicting packages
	for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc; 
	do 
		sudo apt remove $pkg; 
	done

	# setup Docker's apt repository
	# Add Docker's official GPG key:
	sudo apt update
	sudo apt install ca-certificates curl
	sudo install -m 0755 -d /etc/apt/keyrings
	sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
	sudo chmod a+r /etc/apt/keyrings/docker.asc

	# Add the repository to Apt sources:
	echo \
	  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
	  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
	  sudo tee /etc/apt/sources.list.d/docker.sources > /dev/null
	sudo apt update

	# Add the repository to Apt sources:
	# sudo tee /etc/apt/sources.list.d/docker.sources <<EOF \
	# Types: deb \
	# URIs: https://download.docker.com/linux/ubuntu
	# Suites: $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}")
	# Components: stable
	# Signed-By: /etc/apt/keyrings/docker.asc
	# EOF

	sudo apt update

	# install latest version
	sudo apt install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

	# verify that installation worked by starting hell-world
	echo "Starting Docker hello-world to see if setup was successful"
	sudo docker run hello-world
} 
echo "$UBUNTU_CODENAME"

case "$1" in 
	"install-docker")
		install_docker
		;;
esac
