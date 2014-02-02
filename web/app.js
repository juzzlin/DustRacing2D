// This file is part of Dust Racing 2D.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

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

