import requests
import json

def main():
	response = requests.get("https://raw.githubusercontent.com/opengs/uashieldtargets/v2/proxy.json")
	api_list = response.json()
	proxy_file_content = ""
	for api_json in api_list:
		target_site = "https://1c.ru"

		if "ip" not in api_json:
			continue


		proxy_file_content += api_json["ip"].strip() + "| |" + target_site + "\n"


	with open("proxies.txt", "w") as f:
		f.write(proxy_file_content)

main()