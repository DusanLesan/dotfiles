#!/usr/bin/env python

from subprocess import Popen, PIPE
import tempfile
import json
import re

output = errors = ''

def executeCommand(params, failQuit = True):
	try:
		process = Popen(params, stdout=PIPE, stdin=PIPE, stderr=None)
		out = process.communicate()[0].decode('utf-8')
		if failQuit and process.returncode != 0:
			quit()
		return out
	except FileNotFoundError:
		return ''

def processFiles(files):
	for file in files.splitlines():
		validateFile(file)

def validateFile(file):
	global errors
	global output
	global additionalGitArgs
	global blacklist

	if file.endswith(blacklist):
		return

	spellcheck = False
	errors = typos = ''
	lineNum = 0

	changes = executeCommand(['git', 'diff', "-U0"] + additionalGitArgs + ["--"] + [file])

	if executeCommand(['typos', '--version']):
		temp = tempfile.NamedTemporaryFile(mode='w', encoding='utf-8')
		spellcheck = True

	for line in changes.splitlines():
		if line.startswith('@@'):
			num = re.search(r"(?<=\+)\d+", line)
			if num:
				lineNum = int(num.group())

		if line.startswith('+'):
			line = line[1:]

			if spellcheck:
				temp.write(f'{line}\n')

			validateAll(line, lineNum)
			if file.endswith('.brs'):
				validateBRS(line, lineNum)
			elif file.endswith('.xml'):
				validateXML(line, lineNum)

			lineNum += 1

	if spellcheck:
		data = executeCommand([ 'typos', '--format', 'json', '--color', 'never', temp.name ], False)
		temp.close()
		for typo in data.splitlines():
			typo = json.loads(typo)
			typo = f'{typo["typo"]} -> {", ".join(typo["corrections"])}'
			typos += f'{typo}\n'

	if errors or typos:
		output += f'FILE                 : {file}\n'
		if errors:
			output += f'{errors}\n'
		if typos:
			output += f'Misspelled?\n{typos}\n'

def validateLine(text, regex, lineNum, errorMsg):
	regexp = re.compile(regex)
	if regexp.search(text):
		global errors
		errors += errorMsg + ": line " + str(lineNum) + ": " + text + "\n"

def validateAll(line, lineNum):
	validateLine(line, r"\s$", lineNum, 'Trailing whitespaces ')
	validateLine(line, r"\S[ ]{2,}\S", lineNum, 'Multiple spaces      ')
	validateLine(line, r"\t | \t", lineNum, 'Mixed indentation    ')

def validateBRS(line, lineNum):
	validateLine(line, r"(?!.*then)^\s*(else )?if.*$", lineNum, 'If not having then   ')
	validateLine(line, r"function \w*\(.*\)(?! as \w.*)", lineNum, 'No output type       ')
	validateLine(line, r"(sub|function)\s+\w+\((?!(?:\s*\w+\s+(.*?)as\s+\w+(?:\s*,\s*\w+\s+(.*?)as\s+\w+)*)?\s*\))", lineNum, 'No input type        ')

def validateXML(line, lineNum):
	validateLine(line, r"( =|= )", lineNum, 'Bad spacing around = ')

def check(base, branch, blacklist):
	global output
	main([base, branch], blacklist)
	return output

def main(args, blacklistString):
	global additionalGitArgs
	additionalGitArgs = args
	global blacklist
	blacklist = tuple(blacklistString.split(",")) if blacklistString else ()
	changedFiles = executeCommand(['git', 'diff', "--name-only"] + additionalGitArgs)
	processFiles(changedFiles)

if __name__ == '__main__':
	main(['--cached'], '')
	if output:
		print(output)
		exit(1)
	else:
		exit(0)
