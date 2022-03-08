import requests
import json

def main():
	response = requests.get("https://hutin-puy.nadom.app/")
	api_json = response.json()
	proxy_file_content = ""
	for proxy in api_json["proxy"]:
		target_site = "https://1c.ru"

		if "ip" not in proxy:
			continue

		proxy_file_content += proxy["ip"].strip() + "|"
		if "auth" in proxy and proxy["auth"]:
			proxy_file_content += proxy["auth"]
		else:
			proxy_file_content += " "
		proxy_file_content += "|" + target_site + "\n"


	with open("proxies.txt", "w") as f:
		f.write(proxy_file_content)

main()