pipeline {
agent {label 'merlin'}
    stages {
        stage ("build-local-blaze") {
            steps {
                 dir("ws-blaze") {
		 checkout scm
                 script {
//                    git branch: 'release', url: 'git@github.com:falcon-computing/blaze.git'
                        sh "rm -rf release"
                        sh "mkdir release"
                        dir("release"){
//                            sh "rsync -av --exclude=.* /curr/limark/genome-release/build/local/ /curr/limark/falcon2/"
                            sh "cp /curr/limark/genome-release/build/common/tools/bin/sdaccel.ini /curr/limark/falcon2/blaze/bin"
                            sh "source /curr/software/util/modules-tcl/init/bash"
                            sh "module load sdx/17.4; cmake -DCMAKE_BUILD_TYPE=Release -DRELEASE_VERSION=Internal on AWS -DDEPLOYMENT_DST=aws -DCMAKE_INSTALL_PREFIX=/curr/limark/falcon2/blaze .. "
                            sh "make -j 8"
                            sh "make install"
			    sh "cd ~/falcon2;tar zcf blaze-Internal-aws.tgz blaze/; mv blaze-Internal-aws.tgz ~/"
			    link = sh(returnStdout: true, script: 'cd ~/; link=s3://fcs-cicd-test/release/aws/blaze/blaze-Internal-aws.tgz; echo $link; echo $link > latest')
                        	sh "cd ~/; aws s3 cp blaze-Internal-aws.tgz s3://fcs-cicd-test/release/aws/blaze/blaze-Internal-aws.tgz"
                        	sh "cd ~/; aws s3 cp latest s3://fcs-cicd-test/release/aws/blaze/latest"
                        	sh "cd ~/; rm -f latest"
                            }
                        }
                    }
                }
            }
        }
	post {
            always {

                emailext attachLog: true, body: "${currentBuild.currentResult}: Job ${env.JOB_NAME} build ${env.BUILD_NUMBER}\n More info at: ${env.BUILD_URL}console",
                    recipientProviders: [[$class: 'DevelopersRecipientProvider'], [$class: 'RequesterRecipientProvider']],
                    subject: "Jenkins Build ${currentBuild.currentResult}: Job ${env.JOB_NAME}",
                    to: 'udara@limarktech.com, roshantha@limarktech.com'

        }
    }
}    


