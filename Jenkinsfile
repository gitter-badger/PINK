#!groovy

pipeline {
  agent {
    dockerfile {
      filename 'Dockerfile-gcc-4.9'
    }
  }
  stages {
    stage('Build') {
      steps {
        sh '''
          mkdir -p build
          cd build
          cmake ..
          make -j
        '''
      }
    }
    stage('Test') {
      steps {
        script {
          try {
            sh '''
              cd build
              make test
            '''
          } catch (err) {
            echo "Failed: ${err}"
          } finally {
            step([
              $class: 'XUnitBuilder',
              thresholds: [[$class: 'FailedThreshold', unstableThreshold: '1']],
              tools: [[$class: 'GoogleTestType', pattern: 'build/Testing/*.xml']]
            ])
          }
        }
      }
    }
    stage('Doxygen') {
      steps {
        sh '''
          cd build
          make doc
        '''
        publishHTML( target: [
          allowMissing: false,
          alwaysLinkToLastBuild: false,
          keepAll: true,
          reportName: 'Doxygen',
          reportDir: 'doxygen/html',
          reportFiles: 'index.html'
        ])
      }
    }
  }
  post {
    success {
      mail to: 'bernd.doser@h-its.org', subject: "SUCCESS: ${currentBuild.fullDisplayName}", body: "All fine."
    }
    failure {
      mail to: 'bernd.doser@h-its.org', subject: "FAILURE: ${currentBuild.fullDisplayName}", body: "Failed."
    }
  }
}
