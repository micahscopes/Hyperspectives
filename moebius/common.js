document.addEventListener('DOMContentLoaded', function() {
  const hash = window.location.hash
  console.log('hash', hash)
  if (hash && hash=='#simple') {
    document.body.classList.add('simple')
  }
  
  window.addEventListener('resize', function() {
    renderer?.setSize(window.innerWidth, window.innerHeight)
    if (camera) {
      camera.aspect = window.innerWidth / window.innerHeight
      camera.updateProjectionMatrix()
    }
  })
})