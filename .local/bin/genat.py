#!/usr/bin/env python

from time import time
from sys import stderr, exit
from os import environ, urandom
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
from base64 import urlsafe_b64encode

def generateAccessToken(secret: str, key_hex: str) -> str:
	key = bytes.fromhex(key_hex[:64])
	iv = urandom(16)
	nonce = str(int(time()))
	token = f"{nonce}|{secret}".encode('utf-8')

	cipher = AES.new(key, AES.MODE_CBC, iv)
	ciphertext = cipher.encrypt(pad(token, AES.block_size))

	iv_hex = iv.hex()
	cipher_hex = ciphertext.hex()
	iv_len_hex = format(len(iv), '04x').upper()

	output_hex = iv_len_hex + iv_hex + cipher_hex
	output_base64 = urlsafe_b64encode(bytes.fromhex(output_hex)).decode('utf-8')

	return output_base64

if __name__ == "__main__":
	key = environ.get("pp_key")
	secret = environ.get("pp_secret")

	if not key or not secret:
		print("Error: required variables must be set in environment", file=stderr)
		exit(1)

	print(generateAccessToken(secret, key))
