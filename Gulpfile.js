var gulp = require('gulp');
var sass = require('gulp-sass');

const SASS_INCLUDE_PATHS = ['./node_modules/foundation-sites/scss/', './node_modules/motion-ui/']

gulp.task('sass', function() {
    gulp.src('styles.scss')
        .pipe(sass({includePaths: SASS_INCLUDE_PATHS}).on('error', sass.logError))
        .pipe(gulp.dest('dist/css'));
});
