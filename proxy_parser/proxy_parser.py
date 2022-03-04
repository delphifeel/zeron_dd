import requests
import json

def parse_proxy_file(fileName):
	result = ""
	response = requests.get("https://gitlab.com/cto.endel/atack_api/-/raw/master/proxy/" + fileName)
	for line in response.text.splitlines():
		if '@' in line:
			tokens = line.split("@")
			result += tokens[0]+ "|" + tokens[1] + "\n"
		else:
			tokens = line.split(":")
			result += tokens[0] + ":" + tokens[1] + "|" + tokens[2] + ":" + tokens[3] + "\n"

	return result

def main():
	proxy_file_content = ""
	files = [
		"part1.txt",
		"part2.txt",
		"part3.txt",
		"part4.txt",
		"part5.txt"
	]

	for fileName in files:
		proxy_file_content += parse_proxy_file(fileName)


	with open("proxies.txt", "w") as f:
		f.write(proxy_file_content)


main()