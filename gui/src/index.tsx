import { createRoot } from 'react-dom/client';
const App = lazy(() => import('@/App'));
import './reset.css';
import './styles.less';
import React, { lazy } from 'react';

const container = document.getElementById('root');
const root = createRoot(container as HTMLElement);

root.render(<App />);
