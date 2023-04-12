document.addEventListener('DOMContentLoaded', function() {
  const hash = window.location.hash
  console.log('hash', hash)
  if (hash) {
    // document.body.classList.add('simple')
    console.log('removing simple', hash, hash.includes('#simple'))
    if (hash.includes('#translucent-background')) {
      document.body.classList.add('translucent-background')
    }
  } else if (!hash.includes('#simple') || !hash) {
      document.body.classList.remove('simple')
  }
  
  window.addEventListener('resize', function() {
    renderer?.setSize(window.innerWidth, window.innerHeight)
    if (camera) {
      camera.aspect = window.innerWidth / window.innerHeight
      camera.updateProjectionMatrix()
    }
  })
})