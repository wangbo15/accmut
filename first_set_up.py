import os
import getpass

if __name__ == '__main__':
	pwd = os.getcwd()
	cmd0 = 'sudo ln -s ' + pwd + '/tools/accmut/include/ /usr/local/include/accmut'
	cmd1 = 'sudo ln -s ' + pwd + '/tools/accmut/link/ /usr/local/include/accmut_link'
	cmd2 = 'sudo ln -s ' + pwd + '/tools/clang/include/clang/ /usr/local/include/clang'
	os.system(cmd0)
	os.system(cmd1)
	os.system(cmd2)

	home = os.environ['HOME']
	file_path = home + '/tmp/accmut/mutations.txt'

	if os.path.exists(file_path) and not os.path.isfile(file_path):
		print 'ERROR: ' + file_path
		exit()

	if not os.path.exists(file_path):
		os.system('sudo -u ' + getpass.getuser() + ' mkdir -p ' + home + '/tmp/accmut/')
		os.system('sudo -u ' + getpass.getuser() + ' touch ' + file_path)
