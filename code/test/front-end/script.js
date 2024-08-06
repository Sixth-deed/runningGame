document.addEventListener('DOMContentLoaded', function() {
    const yellowBlock = document.getElementById('player');

    document.addEventListener('keydown', function(event) {
        switch (event.key) {
            case 'ArrowUp':
                yellowBlock.style.top = parseInt(yellowBlock.style.top) - 10 + 'px';
                break;
            case 'ArrowDown':
                yellowBlock.style.top = parseInt(yellowBlock.style.top) + 10 + 'px';
                break;
            case 'ArrowLeft':
                yellowBlock.style.left = parseInt(yellowBlock.style.left) - 10 + 'px';
                break;
            case 'ArrowRight':
                yellowBlock.style.left = parseInt(yellowBlock.style.left) + 10 + 'px';
                break;
        }
        // 防止超出屏幕
        if (parseInt(yellowBlock.style.top) < 0) yellowBlock.style.top = '0px';
        if (parseInt(yellowBlock.style.left) < 0) yellowBlock.style.left = '0px';
        const windowHeight = window.innerHeight;
        const windowWidth = window.innerWidth;
        if (parseInt(yellowBlock.style.top) + yellowBlock.offsetHeight > windowHeight) yellowBlock.style.top = (windowHeight - yellowBlock.offsetHeight) + 'px';
        if (parseInt(yellowBlock.style.left) + yellowBlock.offsetWidth > windowWidth) yellowBlock.style.left = (windowWidth - yellowBlock.offsetWidth) + 'px';
    });
});
