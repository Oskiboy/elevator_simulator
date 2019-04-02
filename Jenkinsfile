pipeline {
    agent { 
        dockerfile {
            filename 'Dockerfile.jenkins'
            dir 'Docker'
        }
    }
    stages {
        stage('Initialization') {
            steps {
                dir('logs') {
                    deleteDir()
                    sh '''
                    cat >> build_logs.log <<-EOF
                    BUILD LOGS INITIALIZED!
                    EOF
                    '''
                }
                dir('build') {
                    deleteDir()
                }

            }
        }
        stage('Build') {
            steps {
                sh '''
                echo "Starting build..."
                ./build.sh >> logs/build_logs.log
                '''
                sh 'echo "Build complete..."'
            }
        }
        stage('Wrapper') {
            steps {
                sh '''
                echo "Building wrapper"
                ./build_wrapper.sh >> logs/build_logs.log
                '''
            }
        }
    }
    post {
        success {
            sh '''
            echo "Build passed!"
            '''
            archiveArtifacts artifacts: "build/sim_server, libelev_wrapper.a"
        }
        failure {
            sh '''
            echo "Build failed!"
            '''
        }
        always {
            archiveArtifacts artifacts: "logs/*.log"
            sh '''
            '''
        }
    }
}
