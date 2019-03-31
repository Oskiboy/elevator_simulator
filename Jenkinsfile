pipeline {
    agent { 
        dockerfile {
            filename 'Dockerfile'
            dir 'Docker'
        }
    }
    stages {
        stage('Initialization') {
            steps {
                dir('logs') {
                    deleteDir()
                }

            }
        }
        stage('Build') {
            steps {
                sh '''
                echo "Starting build..."
                ./build.sh
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
            archiveArtifacts artifacts: "logs/*.log, build/heis"
            sh '''
            ls
            echo "Starting clean"
            make clean
            rm -rf build/ logs/
            '''
        }
    }
}
