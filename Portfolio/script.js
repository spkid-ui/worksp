/* * No changes needed. This script works perfectly with the new HTML/CSS.
 */
document.addEventListener('DOMContentLoaded', () => {

    // --- Mobile Navigation Toggle ---
    const navToggle = document.getElementById('nav-toggle');
    const navMenu = document.getElementById('nav-menu');

    if (navToggle && navMenu) {
        navToggle.addEventListener('click', () => {
            navMenu.classList.toggle('active');
            navToggle.classList.toggle('active');
        });
    }

    // --- Smooth Scrolling for all nav links ---
    // Updated query to find the hero section by its new ID '#about'
    const navLinks = document.querySelectorAll('.main-nav a[href^="#"], .hero-links a[href^="#"]');

    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault(); 
            
            const href = this.getAttribute('href');
            // Check for '#' default link, point to 'about' (hero)
            const targetId = href === '#' ? 'about' : href.substring(1);
            const targetElement = document.getElementById(targetId);

            if (targetElement) {
                targetElement.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });

                // If on mobile, close the menu after clicking a link
                if (navMenu.classList.contains('active')) {
                    navMenu.classList.remove('active');
                    navToggle.classList.remove('active');
                }
            }
        });
    });
});

const cards = document.querySelectorAll(".project-card");
  const nextBtn = document.querySelector(".next-btn");
  const prevBtn = document.querySelector(".prev-btn");

  let current = 0;

  function updateCards() {
    const total = cards.length;

    cards.forEach((card, i) => {
      card.classList.remove("card--active", "card--next", "card--prev");
    });

    const next = (current + 1) % total;
    const prev = (current - 1 + total) % total;

    cards[current].classList.add("card--active");
    cards[next].classList.add("card--next");
    cards[prev].classList.add("card--prev");
  }

  nextBtn.addEventListener("click", () => {
    current = (current + 1) % cards.length;
    updateCards();
  });

  prevBtn.addEventListener("click", () => {
    current = (current - 1 + cards.length) % cards.length;
    updateCards();
  });

  updateCards();