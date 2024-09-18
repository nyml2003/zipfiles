import { BrowserRouter, Routes, Route, useRoutes, RouteObject } from 'react-router-dom'
import React, { Suspense, useEffect, useState } from 'react'
import { RouteConfig } from '@/types'
import Fallback from '@/components/Fallback'

const Router = ({ routes }: { routes: RouteObject[] }) => {
  const element = useRoutes(routes)
  return <Suspense fallback={<Fallback />}>{element}</Suspense>
}
export default Router
