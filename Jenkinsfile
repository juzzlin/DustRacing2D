pipeline {
    agent any
    stages {
        stage('CMake Debug build and unit tests') {
            agent {
                docker {
                    image 'juzzlin/qt6-24.04:latest'
                    args '--privileged -t -v $WORKSPACE:/dr2d'
                }
            }
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug -GNinja .."
                sh "cd build-debug && cmake --build . && ctest --output-on-failure"
            }
        }
        stage('Debian package / Ubuntu') {
            matrix {
                axes {
                    axis {
                        name 'UBUNTU_VERSION'
                        values '24.04', '26.04'
                    }
                }
                stages {
                    stage('Build and run CPack') {
                        steps {
                            script {
                                def imageName = "juzzlin/qt6-${UBUNTU_VERSION}:latest"
                                docker.image(imageName).inside('--privileged -t -v $WORKSPACE:/dr2d') {
                                    sh "mkdir -p build-deb-ubuntu-${UBUNTU_VERSION}"
                                    sh "cd build-deb-ubuntu-${UBUNTU_VERSION} && cmake -D DISTRO_VERSION=Ubuntu-${UBUNTU_VERSION} -D ReleaseBuild=ON -D CMAKE_BUILD_TYPE=Release -GNinja .. && cmake --build ."
                                    sh "cd build-deb-ubuntu-${UBUNTU_VERSION} && cpack -G DEB"
                                }
                            }
                        }
                    }
                }
                post {
                    always {
                        archiveArtifacts artifacts: "build-deb-ubuntu-${UBUNTU_VERSION}/*.deb", fingerprint: true
                    }
                }
            }
        }
    }
}

