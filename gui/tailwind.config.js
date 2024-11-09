export default {
  mode: 'jit',
  content: ['./src/**/*.{html,js,jsx,ts,tsx}', './public/index.html'],
  darkMode: 'class',
  theme: {
    extend: {
      width: {
        '25vw': '25vw',
      },
      fontFamily: {
        chinese: ['Noto Sans SC', 'Noto Serif SC', 'Microsoft YaHei', 'SimHei', 'sans-serif'],
      },
    },
  },
  plugins: [],
};
