#!/usr/bin/env python3

import os
import sys
import pathlib
import CppHeaderParser
import git

ROOT_PATH=pathlib.Path(__file__).parent

HEADER_PATH=ROOT_PATH / "gitinfo.h"
GITROOT=ROOT_PATH / "../"

class GitData:
	
	PVAR_COMMIT="GIT_CUR_COMMIT"
	PVAR_BRANCH="GIT_BRANCH"

	def __eq__(self, other):
		if isinstance(other, GitData):
			return self.commit == other.commit and self.branch == other.branch
		return False
		
	def __str__(self):
		return '{} [{}]'.format(self.commit, self.branch)

	def has_commit(self):
		return self.commit != None

	def has_branch(self):
		return self.branch != None
		
	def has_all(self):
		return self.has_commit() and self.has_branch()

	def clear_data(self):
		self.commit = None
		self.branch = None

	def load_from_header(self, hdr_path):

		self.clear_data()

		if not os.path.isfile(hdr_path):
			return

		hparser = CppHeaderParser.CppHeader(hdr_path)

		for e in hparser.defines:
			v = e.split(None,1)

			assert(len(v) == 2)

			if v[0] == self.PVAR_COMMIT:
				self.commit = v[1].lower()
			elif v[0] == self.PVAR_BRANCH:
				self.branch = v[1].strip('"').lower()

	def load_from_repo(self, root_path):

		self.clear_data()

		repo = git.Repo(path=root_path)

		active_branch = repo.active_branch.name

		commits = list(repo.iter_commits(active_branch, max_count=1))

		assert len(commits) == 1

		self.branch = active_branch.lower()
		self.commit = commits[0].hexsha[0:8].lower()

	def write_header(self, hdr_path):

		if not self.has_all():
			raise Exception("missing data")

		with open(hdr_path, 'w') as f:
			f.write('#define {} {}\n'.format(self.PVAR_COMMIT, self.commit))
			f.write('#define {} "{}"\n'.format(self.PVAR_BRANCH, self.branch))
		

datah = GitData()
datah.load_from_header(HEADER_PATH)

print('hdr: {}'.format(datah))

datar = GitData()
datar.load_from_repo(GITROOT)

print('cur: {}'.format(datar))

if datah != datar:
	print('writing {}'.format(HEADER_PATH))
	datar.write_header(HEADER_PATH)

sys.exit(0)