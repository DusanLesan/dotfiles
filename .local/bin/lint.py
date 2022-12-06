#!/usr/bin/env python

from subprocess import Popen, PIPE
import re

output = errors = ''

def executeCommand(params, input = ''):
	process = Popen(params, stdout=PIPE, stdin=PIPE, stderr=None)
	out = process.communicate(input=input.encode())[0].decode('utf-8')
	if process.returncode != 0:
		quit()
	return out

def processFiles(files):
	for file in files.splitlines():
		validateFile(file)

def validateFile(file):
	global errors
	global output
	global additionalGitArgs
	errors = ''
	lineNum = 0

	changes = executeCommand(['git', 'diff', "-U0"] + additionalGitArgs + [file])

	for line in changes.splitlines():
		if line.startswith('@@'):
			num = re.search(r"(?<=\+)\d+", line)
			if num:
				lineNum = int(num.group())

		if line.startswith('+'):
			line = line[1:]

			validateAll(line, lineNum)
			if file.endswith('.brs'):
				validateBRS(line, lineNum)
			elif file.endswith('.xml'):
				validateXML(line, lineNum)

			lineNum += 1
	if errors:
		output += 'FILE                 : ' + file + '\n' + errors + '\n'

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

def check(base, branch):
	global output
	main([base, branch])
	return output

def main(args):
	global additionalGitArgs
	additionalGitArgs = args
	changedFiles = executeCommand(['git', 'diff', "--name-only"] + additionalGitArgs)
	processFiles(changedFiles)

if __name__ == '__main__':
	main(['--cached'])
	print(output)
	if output:
		exit(1)
	else:
		exit(0)
