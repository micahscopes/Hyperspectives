document.addEventListener('DOMContentLoaded', function() {
  const hash = window.location.hash
  if (hash && hash=='simple') {
    document.html.classList.add('simple')
  }
})