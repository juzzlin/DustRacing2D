var gulp = require('gulp');
var sass = require('gulp-sass');
var concat = require('gulp-concat');

const SASS_INCLUDE_PATHS = ['./node_modules/foundation-sites/scss/', './node_modules/motion-ui/']

gulp.task('sass', function() {
    gulp.src('styles.scss')
        .pipe(sass({includePaths: SASS_INCLUDE_PATHS}).on('error', sass.logError))
        .pipe(gulp.dest('dist/css'));
});

const SCRIPTS = [
    "node_modules/jquery/dist/jquery.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.core.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.util.mediaQuery.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.util.keyboard.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.util.motion.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.util.timerAndImageLoader.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.util.touch.min.js",
    "node_modules/foundation-sites/dist/js/plugins/foundation.orbit.min.js",
    "node_modules/motion-ui/dist/motion-ui.min.js"];

gulp.task('scripts', function() {
    return gulp.src(SCRIPTS)
        .pipe(concat('vendor.js'))
        .pipe(gulp.dest('./dist/js'));
});

// Default task
gulp.task('default', ['sass', 'scripts']);