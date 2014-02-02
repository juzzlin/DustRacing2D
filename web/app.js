var app = angular.module("App", []);

app.controller("ImageController", function ($scope, $timeout) {
    $scope.image1 = 'screenshots/1.6.3/1_600x333.jpg';
    $scope.image2 = 'screenshots/1.6.3/2_600x333.jpg';

    var imageIndex = 0;
    var images = [
        'screenshots/1.6.3/3_600x333.jpg',
        'screenshots/1.6.3/4_600x333.jpg',
        'screenshots/1.6.3/5_600x333.jpg',
        'screenshots/1.6.3/6_600x333.jpg',
        'screenshots/1.6.3/7_600x333.jpg',
        'screenshots/1.6.3/8_600x333.jpg'];

    $scope.image3 = images[imageIndex];
    $scope.intervalFunction = function() {
        $timeout(function() {
            if (++imageIndex >= images.length) {
                imageIndex = 0;
            }
            $scope.image3 = images[imageIndex];
            $scope.intervalFunction();
        }, 5000)
    };

    $scope.intervalFunction();
});

