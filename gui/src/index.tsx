import { createRoot } from 'react-dom/client';
import App from './App';
import './reset.css';
import './styles.less';
import React from 'react';

let container = document.getElementById('root');
const root = createRoot(container as HTMLElement);



root.render(<App />)
