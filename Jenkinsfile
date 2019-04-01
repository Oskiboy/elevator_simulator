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
                    cat >> build_logs.log <<EOF
                    BUILD LOGS INITIALIZED!
                    EOF
                    '''
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
    }
    post {
        success {
            sh '''
            echo "Build passed!"
            '''
        }
        failure {
            sh '''
            echo "Build failed!"
            '''
        }
        always {
            archiveArtifacts artifacts: "logs/*.log, build/sim_server"
            sh '''
            ls
            echo "Starting clean"
            make clean
            rm -rf build/ logs/
            '''
        }
    }
}
