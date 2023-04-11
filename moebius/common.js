document.addEventListener('DOMContentLoaded', function() {
  const hash = window.location.hash
  console.log('hash', hash)
  if (hash && hash=='#simple') {
    document.body.classList.add('simple')
  }
})