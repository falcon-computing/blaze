pipeline {
agent {label 'merlin'}
    stages {
        stage ("build-aws-blaze") {
            steps {
                 dir("ws-blaze") {
		 checkout scm
                 script {
                        sh "rm -rf release"
                        sh "mkdir release"
                        dir("release"){
//                            sh "rsync -av --exclude=.* /curr/limark/genome-release/build/local/ /curr/limark/falcon2/"
                       	sh "cp /curr/limark/genome-release/build/common/tools/bin/sdaccel.ini /curr/limark/falcon2/blaze/bin"
                       	sh "source /curr/software/util/modules-tcl/init/bash"
			version= sh(returnStdout: true, script: 'git describe --tag').trim()
                        sh "echo $version"
                        sh "module load sdx/17.4; cmake -DCMAKE_BUILD_TYPE=Release -DRELEASE_VERSION=$version -DDEPLOYMENT_DST=aws -DCMAKE_INSTALL_PREFIX=/curr/limark/falcon2/blaze .. "
                        sh "make -j 8"
                        sh "make install"
			sh "cd ~/falcon2;tar zcf blaze-$version-aws.tgz blaze/; mv blaze-$version-aws.tgz ~/artifacts"
			sh "cd ~/artifacts; echo s3://fcs-cicd-test/release/aws/blaze/blaze-$version-aws.tgz > latest"
			sh "cd ~/artifacts; aws s3 cp blaze-$version-aws.tgz s3://fcs-cicd-test/release/aws/blaze/blaze-$version-aws.tgz"
                        sh "cd ~/; aws s3 cp latest s3://fcs-cicd-test/release/aws/blaze/latest"
                        sh "cd ~/; rm -f latest"
			}
		     }
                  }
               }
            }
        }
    }     


