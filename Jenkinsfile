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
            archiveArtifacts artifacts: "build/sim_server"
        }
        failure {
            sh '''
            echo "Build failed!"
            '''
        }
        always {
            archiveArtifacts artifacts: "logs/*.log"
            sh '''
            ls
            echo "Starting clean"
            rm -rf build/ logs/
            '''
        }
    }
}
