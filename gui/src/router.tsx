import { RouteObject } from 'react-router-dom';
import React, { lazy, Suspense } from 'react';

const EntryLayout = lazy(() => import('./layouts/EntryLayout'));
const ExplorerPage = lazy(() => import('./pages/ExplorerPage'));
const NotFound = lazy(() => import('./components/NotFound'));

const routes: RouteObject[] = [
  {
    path: '/',
    element: (
      <Suspense fallback={<div>Loading...</div>}>
        <EntryLayout />
      </Suspense>
    ),
    children: [
      {
        path: 'explorer',
        element: (
          <Suspense fallback={<div>Loading...</div>}>
            <ExplorerPage />
          </Suspense>
        ),
      },
    ],
  },
  {
    path: '*',
    element: (
      <Suspense fallback={<div>Loading...</div>}>
        <NotFound />
      </Suspense>
    ),
  },
];

export default routes;
