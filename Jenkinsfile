pipeline {
    agent any
    environment {
        BUILD_VERSION = "${BRANCH_NAME}-${BUILD_NUMBER}"
        HERE_TRACKING_TEST_DEVICE_ID = credentials('HERE_TRACKING_TEST_DEVICE_ID')
        HERE_TRACKING_TEST_DEVICE_SECRET = credentials('HERE_TRACKING_TEST_DEVICE_SECRET')
    }
    stages {
        stage('Build') {
            steps {
                sh './build.sh'
            }
        }
        stage('Test'){
            steps {
                sh "export TEST=1 \
                    HERE_TRACKING_TEST_DEVICE_ID=${env.HERE_TRACKING_TEST_DEVICE_ID} \
                    HERE_TRACKING_TEST_DEVICE_SECRET=${env.HERE_TRACKING_TEST_DEVICE_SECRET} && \
                    ./build.sh"
                step([$class: 'XUnitBuilder',
                     thresholds: [[$class: 'FailedThreshold', unstableThreshold: '1']],
                     tools: [[$class: 'CheckType', pattern: '**/build/**/*_test_result.xml']]])
                step([$class: 'CoberturaPublisher',
                      coberturaReportFile: 'build/coverage/coverage.xml'])
            }
        }
        stage('Doxygen'){
            steps {
                sh 'export DOXYGEN=1 && ./build.sh'
                zip zipFile: 'doc.zip', archive: true, dir: 'build/doc'
            }
        }
    }
    post {
        always {
            echo 'Cleanup'
            step([$class: 'WsCleanup'])
        }
        success {
            echo 'Success!!!'
        }
        failure {
            echo 'Failure ;('
        }
        unstable {
            echo 'Unstable'
        }
        changed {
            echo 'State changed'
        }
    }
}